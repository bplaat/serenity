/*
htop - serenity/SerenityProcessTable.c
(C) 2026 SerenityOS contributors
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "serenity/SerenityProcessTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Macros.h"
#include "Object.h"
#include "Process.h"
#include "ProcessTable.h"
#include "Settings.h"
#include "XUtils.h"
#include "serenity/SerenityMachine.h"
#include "serenity/SerenityProcess.h"
#include "serenity/jsmn.h"

#define HTOP_LOG "/tmp/htop-serenity.log"
static inline void htop_log(const char* msg) {
   FILE* f = fopen(HTOP_LOG, "a");
   if (f) { fputs(msg, f); fputc('\n', f); fclose(f); }
}


/* Maximum number of jsmn tokens.  We allocate dynamically if exceeded. */
#define JSMN_TOKENS_INITIAL 65536

/* ---------- jsmn token array management ---------- */

typedef struct {
   jsmntok_t* tokens;
   int        count;
} TokenBuf;

static int TokenBuf_parse(TokenBuf* tb, const char* json, size_t len) {
   int capacity = JSMN_TOKENS_INITIAL;

   if (!tb->tokens)
      tb->tokens = xMalloc((size_t)capacity * sizeof(jsmntok_t));

   for (;;) {
      jsmn_parser p;
      jsmn_init(&p);
      int r = jsmn_parse(&p, json, len, tb->tokens, (unsigned int)capacity);
      if (r >= 0) {
         tb->count = r;
         return 0;
      }
      if (r == JSMN_ERROR_NOMEM) {
         capacity *= 2;
         tb->tokens = xRealloc(tb->tokens, (size_t)capacity * sizeof(jsmntok_t));
         continue;
      }
      return -1; /* JSMN_ERROR_INVAL or JSMN_ERROR_PART */
   }
}

/* ---------- JSON helpers ---------- */

static unsigned long long tok_ull(const char* json, const jsmntok_t* tok) {
   return jsmn_ull(json, tok);
}

static void tok_str(const char* json, const jsmntok_t* tok, char* buf, size_t bufsz) {
   jsmn_strcpy(json, tok, buf, bufsz);
}

/* Map a SerenityOS thread state string to htop ProcessState enum */
static ProcessState parseState(const char* stateStr) {
   if (strcmp(stateStr, "Running")     == 0) return RUNNING;
   if (strcmp(stateStr, "Runnable")    == 0) return RUNNABLE;
   if (strcmp(stateStr, "Sleeping")    == 0) return SLEEPING;
   if (strcmp(stateStr, "Stopped")     == 0) return STOPPED;
   if (strcmp(stateStr, "Zombie")      == 0) return ZOMBIE;
   if (strcmp(stateStr, "Idle")        == 0) return IDLE;
   if (strcmp(stateStr, "Blocked")     == 0) return BLOCKED;
   if (strcmp(stateStr, "Queued")      == 0) return QUEUED;
   if (strcmp(stateStr, "Dead")        == 0) return ZOMBIE;
   return UNKNOWN;
}

/* ---------- Per-process JSON parsing ---------- */

typedef struct {
   /* From process object */
   pid_t pid;
   pid_t ppid;
   pid_t pgid;
   pid_t sid;
   uid_t uid;
   bool  kernel;
   char  name[128];
   char  executable[512];
   char  tty[64];
   char  pledge[256];
   char  veil[256];
   memory_t m_virt;          /* bytes → will convert to KB */
   memory_t m_resident;      /* bytes → will convert to KB */
   time_t creation_time;     /* nanoseconds since epoch */

   /* Aggregated from threads */
   unsigned long long time_user;
   unsigned long long time_kernel;
   unsigned long long inode_faults;
   unsigned long long zero_faults;
   unsigned long long cow_faults;
   int nlwp;
   ProcessState state;       /* from first (or any running) thread */
   unsigned int cpu;         /* processor from first running thread */
   int priority;
} ProcInfo;

/*
 * Parse a single process JSON object starting at tokens[procIdx].
 * procIdx points to the JSMN_OBJECT token for the process.
 * Returns the number of tokens consumed.
 */
static int parseProcessObject(const char* json, const jsmntok_t* tokens, int procIdx, ProcInfo* info) {
   memset(info, 0, sizeof(*info));
   info->state    = UNKNOWN;
   info->priority = 30;

   const jsmntok_t* proc = &tokens[procIdx];
   if (proc->type != JSMN_OBJECT)
      return jsmn_subtree_size(tokens, procIdx);

   int i = procIdx + 1;

   for (int k = 0; k < proc->size; k++) {
      const jsmntok_t* key = &tokens[i];
      const jsmntok_t* val = &tokens[i + 1];
      i += 2;

      if (jsmn_streq(json, key, "pid")) {
         info->pid = (pid_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "ppid")) {
         info->ppid = (pid_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "pgp")) {
         info->pgid = (pid_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "sid")) {
         info->sid = (pid_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "uid")) {
         info->uid = (uid_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "kernel")) {
         /* "true" or "false" primitive */
         info->kernel = (json[val->start] == 't');
      } else if (jsmn_streq(json, key, "name")) {
         tok_str(json, val, info->name, sizeof(info->name));
      } else if (jsmn_streq(json, key, "executable")) {
         tok_str(json, val, info->executable, sizeof(info->executable));
      } else if (jsmn_streq(json, key, "tty")) {
         tok_str(json, val, info->tty, sizeof(info->tty));
      } else if (jsmn_streq(json, key, "pledge")) {
         tok_str(json, val, info->pledge, sizeof(info->pledge));
      } else if (jsmn_streq(json, key, "veil")) {
         tok_str(json, val, info->veil, sizeof(info->veil));
      } else if (jsmn_streq(json, key, "amount_virtual")) {
         info->m_virt = (memory_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "amount_resident")) {
         info->m_resident = (memory_t)tok_ull(json, val);
      } else if (jsmn_streq(json, key, "creation_time")) {
         info->creation_time = (time_t)(tok_ull(json, val) / 1000000000ULL);
      } else if (jsmn_streq(json, key, "threads")) {
         /* val is JSMN_ARRAY of thread objects */
         if (val->type != JSMN_ARRAY) {
            i += jsmn_subtree_size(tokens, i - 1) - 1;
            continue;
         }

         int tidx = i; /* first thread token */
         for (int t = 0; t < val->size; t++) {
            const jsmntok_t* tobj = &tokens[tidx];
            if (tobj->type != JSMN_OBJECT) {
               tidx += jsmn_subtree_size(tokens, tidx);
               continue;
            }

            int ti = tidx + 1;
            unsigned long long thread_user   = 0;
            unsigned long long thread_kernel = 0;
            unsigned long long iflt = 0, zflt = 0, cflt = 0;
            char thread_state[32] = {0};
            unsigned int thread_cpu = 0;
            int thread_prio = 30;

            for (int tk = 0; tk < tobj->size; tk++) {
               const jsmntok_t* tk_key = &tokens[ti];
               const jsmntok_t* tk_val = &tokens[ti + 1];
               ti += 2;

               if (jsmn_streq(json, tk_key, "time_user")) {
                  thread_user = tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "time_kernel")) {
                  thread_kernel = tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "inode_faults")) {
                  iflt = tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "zero_faults")) {
                  zflt = tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "cow_faults")) {
                  cflt = tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "state")) {
                  tok_str(json, tk_val, thread_state, sizeof(thread_state));
               } else if (jsmn_streq(json, tk_key, "cpu")) {
                  thread_cpu = (unsigned int)tok_ull(json, tk_val);
               } else if (jsmn_streq(json, tk_key, "priority")) {
                  thread_prio = (int)tok_ull(json, tk_val);
               } else {
                  /* skip unknown value */
                  ti += jsmn_subtree_size(tokens, ti - 1) - 1;
               }
            }

            info->time_user   += thread_user;
            info->time_kernel += thread_kernel;
            info->inode_faults += iflt;
            info->zero_faults  += zflt;
            info->cow_faults   += cflt;
            info->nlwp++;

            ProcessState ts = parseState(thread_state);
            /* Prefer "Running" state if any thread is running */
            if (info->state == UNKNOWN || ts == RUNNING) {
               info->state = ts;
               info->cpu   = thread_cpu;
            }
            info->priority = thread_prio;

            tidx += jsmn_subtree_size(tokens, tidx);
         }
         i = tidx; /* advance past the threads array content */
         /* we already advanced i by 2 (key+val) above; val was the array root,
          * so we need to rewind by 1 and use tidx */
         i = tidx;
         continue; /* already advanced i */
      } else {
         /* skip unknown value subtree */
         i += jsmn_subtree_size(tokens, i - 1) - 1;
      }
   }

   return i - procIdx;
}

/* ---------- htop ProcessTable interface ---------- */

ProcessTable* ProcessTable_new(Machine* host, Hashtable* pidMatchList) {
   SerenityProcessTable* this = xCalloc(1, sizeof(SerenityProcessTable));
   Object_setClass(this, Class(ProcessTable));

   ProcessTable* super = &this->super;
   ProcessTable_init(super, Class(SerenityProcess), host, pidMatchList);

   return super;
}

void ProcessTable_delete(Object* cast) {
   SerenityProcessTable* this = (SerenityProcessTable*) cast;
   ProcessTable_done(&this->super);
   free(this);
}

void ProcessTable_goThroughEntries(ProcessTable* super) {
   const Machine*          host    = super->super.host;
   const SerenityMachine*  shost   = (const SerenityMachine*) host;
   const Settings*         settings = host->settings;
   bool hideKernelThreads   = settings->hideKernelThreads;
   bool hideUserlandThreads = settings->hideUserlandThreads;

   /* Ensure Machine_scan has populated the buffer */
   if (!shost->procBuf || shost->procBufLen == 0) {
      htop_log("ProcessTable_goThroughEntries: procBuf empty, skipping");
      return;
   }

   const char* json    = shost->procBuf;
   size_t      jsonLen = shost->procBufLen;

   /* Total scheduled time delta for CPU% calculations.
    * Machine_scan computes this delta BEFORE updating prevTotalTime,
    * so it is valid here even though Machine_scan already ran. */
   unsigned long long totalDelta = shost->totalTimeDelta;

   /* Tokenize the JSON */
   static TokenBuf tb = { NULL, 0 };
   if (TokenBuf_parse(&tb, json, jsonLen) < 0) {
      htop_log("ProcessTable_goThroughEntries: JSON parse FAILED");
      return;
   }
   {
      char buf[64];
      snprintf(buf, sizeof(buf), "ProcessTable_goThroughEntries: %d tokens, delta=%llu", tb.count, totalDelta);
      htop_log(buf);
   }

   if (tb.count < 1 || tb.tokens[0].type != JSMN_OBJECT)
      return;

   /* Walk top-level object looking for "processes" array */
   int rootSize = tb.tokens[0].size;
   int i = 1;
   for (int k = 0; k < rootSize; k++) {
      const jsmntok_t* key = &tb.tokens[i];
      const jsmntok_t* val = &tb.tokens[i + 1];

      if (jsmn_streq(json, key, "processes") && val->type == JSMN_ARRAY) {
         {
            char buf[64];
            snprintf(buf, sizeof(buf), "processes array: size=%d", val->size);
            htop_log(buf);
         }
         int pidx = i + 2; /* first process object */
         for (int p = 0; p < val->size; p++) {
            ProcInfo info;
            int consumed = parseProcessObject(json, tb.tokens, pidx, &info);
            {
               char buf[128];
               snprintf(buf, sizeof(buf), "process[%d]: pid=%d name='%s' kernel=%d nlwp=%d", p, info.pid, info.name, info.kernel, info.nlwp);
               htop_log(buf);
            }

            /* Get or create htop Process entry */
            bool preExisting = false;
            Process* proc = ProcessTable_getProcess(super, info.pid, &preExisting, SerenityProcess_new);
            SerenityProcess* sp = (SerenityProcess*)proc;

            if (!preExisting) {
               Process_setPid(proc, info.pid);
               Process_setThreadGroup(proc, info.pid);
               Process_setParent(proc, info.ppid);
               proc->isKernelThread  = info.kernel;
               proc->isUserlandThread = false;
               proc->pgrp            = info.pgid;
               proc->session         = info.sid;
               proc->st_uid          = info.uid;
               proc->starttime_ctime = info.creation_time > 0 ? info.creation_time : (time_t)(host->realtimeMs / 1000);
               Process_fillStarttimeBuffer(proc);
               proc->user = UsersTable_getRef(host->usersTable, proc->st_uid);
               ProcessTable_add(super, proc);

               if (info.executable[0])
                  Process_updateExe(proc, info.executable);
               if (info.name[0]) {
                  Process_updateComm(proc, info.name);
                  Process_updateCmdline(proc, info.name, 0, (int)strlen(info.name));
               }

               if (info.tty[0] && strcmp(info.tty, "?") != 0) {
                  free_and_xStrdup(&proc->tty_name, info.tty);
               }
            } else {
               /* Update mutable fields */
               Process_setParent(proc, info.ppid);
               if (proc->st_uid != info.uid) {
                  proc->st_uid = info.uid;
                  proc->user = UsersTable_getRef(host->usersTable, proc->st_uid);
               }
               if (settings->updateProcessNames && info.name[0]) {
                  Process_updateComm(proc, info.name);
                  Process_updateCmdline(proc, info.name, 0, (int)strlen(info.name));
               }
            }

            /* Always-updated fields */
            proc->m_virt     = info.m_virt     / 1024ULL; /* bytes → KB */
            proc->m_resident = info.m_resident / 1024ULL;
            proc->nlwp       = info.nlwp;
            proc->state      = (info.state != UNKNOWN) ? info.state : SLEEPING;
            proc->processor  = info.cpu;
            proc->priority   = info.priority;
            proc->nice       = PROCESS_NICE_UNKNOWN;
            proc->majflt     = sp->cowFaults;

            /* CPU% from per-process time delta vs total scheduler delta */
            unsigned long long threadTime = info.time_user + info.time_kernel;
            unsigned long long threadDelta = (threadTime >= sp->prevThreadTime)
                                             ? threadTime - sp->prevThreadTime : 0;
            sp->prevThreadTime = threadTime;
            if (totalDelta > 0) {
               proc->percent_cpu = 100.0 * (double)threadDelta / (double)totalDelta;
               if (proc->percent_cpu > 100.0) proc->percent_cpu = 100.0;
               if (proc->percent_cpu < 0.0)   proc->percent_cpu = 0.0;
            } else {
               proc->percent_cpu = 0.0;
            }

            if (host->totalMem > 0)
               proc->percent_mem = 100.0 * (double)proc->m_resident / (double)host->totalMem;
            else
               proc->percent_mem = 0.0;

            Process_updateCPUFieldWidths(proc->percent_cpu);

            /* CPU time in centiseconds (thread time is in nanoseconds) */
            proc->time = (unsigned long long)(info.time_user + info.time_kernel) / 10000000ULL;

            /* SerenityOS-specific fields */
            if (info.pledge[0])
               free_and_xStrdup(&sp->pledge, info.pledge);
            else {
               free(sp->pledge);
               sp->pledge = NULL;
            }
            if (info.veil[0])
               free_and_xStrdup(&sp->veil, info.veil);
            else {
               free(sp->veil);
               sp->veil = NULL;
            }
            sp->inodeFaults = info.inode_faults;
            sp->zeroFaults  = info.zero_faults;
            sp->cowFaults   = info.cow_faults;
            sp->isKernelProc = info.kernel;

            /* Visibility */
            proc->super.show = !((hideKernelThreads   && Process_isKernelThread(proc)) ||
                                 (hideUserlandThreads  && Process_isUserlandThread(proc)));

            super->totalTasks++;
            if (proc->state == RUNNING)
               super->runningTasks++;
            if (Process_isKernelThread(proc))
               super->kernelThreads++;

            proc->super.updated = true;
            pidx += consumed;
         }
         {
            char buf[64];
            snprintf(buf, sizeof(buf), "scan done: totalTasks=%u runningTasks=%u", super->totalTasks, super->runningTasks);
            htop_log(buf);
         }
         break;
      }

      /* skip key + value */
      i += 2 + jsmn_subtree_size(tb.tokens, i + 1) - 1;
   }
}
