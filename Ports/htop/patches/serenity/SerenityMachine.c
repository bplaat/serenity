/*
htop - serenity/SerenityMachine.c
(C) 2026 SerenityOS contributors
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "serenity/SerenityMachine.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define HTOP_LOG "/tmp/htop-serenity.log"

static FILE* htop_log_file(void) {
   static FILE* f = NULL;
   if (!f) f = fopen(HTOP_LOG, "a");
   return f;
}

#define HTOP_LOG_WRITE(fmt, ...) do { \
   FILE* _f = htop_log_file(); \
   if (_f) { fprintf(_f, fmt "\n", ##__VA_ARGS__); fflush(_f); } \
} while (0)

#include "CRT.h"
#include "Macros.h"
#include "XUtils.h"
#include "serenity/jsmn.h"


/* Minimum buffer size for /sys/kernel/processes JSON */
#define PROC_BUF_INITIAL_SIZE (64 * 1024)   /* 64 KiB */
#define PROC_JSON_PATH         "/sys/kernel/processes"

/*
 * Read the entire content of PROC_JSON_PATH into host->procBuf,
 * growing the buffer as needed.  Returns 0 on success.
 */
static int SerenityMachine_readProcBuf(SerenityMachine* host) {
   int fd = open(PROC_JSON_PATH, O_RDONLY);
   if (fd < 0)
      return -1;

   if (!host->procBuf || host->procBufCapacity < PROC_BUF_INITIAL_SIZE) {
      free(host->procBuf);
      host->procBuf = xMalloc(PROC_BUF_INITIAL_SIZE);
      host->procBufCapacity = PROC_BUF_INITIAL_SIZE;
   }

   size_t total = 0;
   for (;;) {
      ssize_t n = read(fd, host->procBuf + total, host->procBufCapacity - total - 1);
      if (n < 0) {
         close(fd);
         return -1;
      }
      if (n == 0)
         break;
      total += (size_t)n;
      if (total + 1 >= host->procBufCapacity) {
         size_t newCap = host->procBufCapacity * 2;
         char* newBuf = xRealloc(host->procBuf, newCap);
         host->procBuf = newBuf;
         host->procBufCapacity = newCap;
      }
   }
   close(fd);

   host->procBuf[total] = '\0';
   host->procBufLen = total;
   return 0;
}

/*
 * Extract total_time and total_time_kernel from the top-level JSON object.
 * These keys only appear once at the top level, so strstr is safe.
 */
static void SerenityMachine_parseTimes(SerenityMachine* host) {
   const char* json = host->procBuf;
   const char* p;

   p = strstr(json, "\"total_time_kernel\":");
   if (p) {
      p += strlen("\"total_time_kernel\":");
      while (*p == ' ' || *p == '\t') p++;
      unsigned long long v = 0;
      while (*p >= '0' && *p <= '9') {
         v = v * 10 + (unsigned long long)(*p - '0');
         p++;
      }
      host->curKernelTime = v;
   }

   p = strstr(json, "\"total_time\":");
   if (p) {
      p += strlen("\"total_time\":");
      while (*p == ' ' || *p == '\t') p++;
      unsigned long long v = 0;
      while (*p >= '0' && *p <= '9') {
         v = v * 10 + (unsigned long long)(*p - '0');
         p++;
      }
      host->curTotalTime = v;
   }
}

/*
 * Sum all "amount_resident" values in the JSON to estimate used memory.
 * This gives the total resident set of all processes in bytes, which we
 * convert to KB for htop's memory_t type.
 */
static memory_t SerenityMachine_sumResidentMem(const char* json) {
   const char* needle = "\"amount_resident\":";
   size_t nlen = strlen(needle);
   memory_t total = 0;
   const char* p = json;

   while ((p = strstr(p, needle)) != NULL) {
      p += nlen;
      while (*p == ' ' || *p == '\t') p++;
      unsigned long long v = 0;
      while (*p >= '0' && *p <= '9') {
         v = v * 10 + (unsigned long long)(*p - '0');
         p++;
      }
      total += (memory_t)(v / 1024ULL); /* bytes → KB */
   }
   return total;
}

/*
 * Update the aggregate CPU percentage from the time deltas.
 */
static void SerenityMachine_updateCPUData(SerenityMachine* host) {
   /* Compute delta BEFORE updating prev so ProcessTable can use it */
   unsigned long long totalDelta  = (host->curTotalTime  > host->prevTotalTime)
                                    ? host->curTotalTime  - host->prevTotalTime : 0;
   unsigned long long kernelDelta = (host->curKernelTime > host->prevKernelTime)
                                    ? host->curKernelTime - host->prevKernelTime : 0;

   host->totalTimeDelta = totalDelta;

   CPUData* cpu = &host->cpus[0]; /* aggregate, also used for all per-core slots */

   if (totalDelta > 0) {
      cpu->kernelPercent = 100.0 * (double)kernelDelta / (double)totalDelta;
      if (cpu->kernelPercent > 100.0) cpu->kernelPercent = 100.0;
      cpu->userPercent   = 100.0 - cpu->kernelPercent;
      if (cpu->userPercent < 0.0) cpu->userPercent = 0.0;
      cpu->idlePercent   = 0.0;
   } else {
      cpu->userPercent   = 0.0;
      cpu->kernelPercent = 0.0;
      cpu->idlePercent   = 100.0;
   }
   cpu->systemAllPercent = cpu->kernelPercent + cpu->userPercent;

   const Machine* super = &host->super;
   for (unsigned int i = 1; i <= super->existingCPUs; i++) {
      host->cpus[i] = *cpu;
   }

   host->prevTotalTime  = host->curTotalTime;
   host->prevKernelTime = host->curKernelTime;
}

/* ---------- htop Machine interface ---------- */

Machine* Machine_new(UsersTable* usersTable, uid_t userId) {
   SerenityMachine* this = xCalloc(1, sizeof(SerenityMachine));
   Machine* super = &this->super;

   Machine_init(super, usersTable, userId);

   /* Detect CPU count via POSIX sysconf */
   long cpus = sysconf(_SC_NPROCESSORS_CONF);
   if (cpus < 1) cpus = 1;

   super->existingCPUs = (unsigned int)cpus;
   super->activeCPUs   = (unsigned int)cpus;

   /* Allocate CPUData: index 0 = aggregate, 1..cpus = per-core */
   this->cpus = xCalloc((size_t)cpus + 1, sizeof(CPUData));

   /* Initial total memory value */
   long pageSize   = sysconf(_SC_PAGESIZE);
   long totalPages = sysconf(_SC_PHYS_PAGES);
   if (pageSize > 0 && totalPages > 0)
      super->totalMem = (memory_t)totalPages * (memory_t)pageSize / 1024ULL;

   return super;
}

void Machine_delete(Machine* cast) {
   SerenityMachine* this = (SerenityMachine*) cast;
   free(this->cpus);
   free(this->procBuf);
   Machine_done(cast);
   free(this);
}

bool Machine_isCPUonline(const Machine* this, unsigned int id) {
   return id < this->existingCPUs;
}

void Machine_scan(Machine* cast) {
   SerenityMachine* this = (SerenityMachine*) cast;

   /* Read /sys/kernel/processes into procBuf (shared with ProcessTable) */
   if (SerenityMachine_readProcBuf(this) < 0) {
      HTOP_LOG_WRITE("Machine_scan: readProcBuf FAILED (errno=%d)", errno);
      return;
   }
   HTOP_LOG_WRITE("Machine_scan: read %zu bytes from " PROC_JSON_PATH, this->procBufLen);

   /* Extract global CPU time totals from the JSON */
   SerenityMachine_parseTimes(this);

   /* Refresh total memory */
   long pageSize   = sysconf(_SC_PAGESIZE);
   long totalPages = sysconf(_SC_PHYS_PAGES);
   if (pageSize > 0 && totalPages > 0)
      cast->totalMem = (memory_t)totalPages * (memory_t)pageSize / 1024ULL;

   /* Estimate used memory by summing process resident sets */
   cast->usedMem = SerenityMachine_sumResidentMem(this->procBuf);
   if (cast->usedMem > cast->totalMem)
      cast->usedMem = cast->totalMem;

   /* Update CPU percentages from the new time data */
   SerenityMachine_updateCPUData(this);
}
