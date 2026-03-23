/*
 * jsmn.h - Minimal JSON tokenizer
 * MIT License - Original by Serge Zaitsev
 * Bundled for htop SerenityOS platform support.
 */
#ifndef HEADER_jsmn
#define HEADER_jsmn

#include <stddef.h>
#include <string.h>

typedef enum {
   JSMN_UNDEFINED = 0,
   JSMN_OBJECT    = 1,
   JSMN_ARRAY     = 2,
   JSMN_STRING    = 3,
   JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
   JSMN_ERROR_NOMEM = -1, /* Not enough tokens */
   JSMN_ERROR_INVAL = -2, /* Invalid character */
   JSMN_ERROR_PART  = -3  /* JSON string is too short */
};

typedef struct jsmntok_ {
   jsmntype_t type;
   int start;
   int end;
   int size;
   int parent;
} jsmntok_t;

typedef struct jsmn_parser_ {
   unsigned int pos;
   unsigned int toknext;
   int toksuper;
} jsmn_parser;

static inline void jsmn_init(jsmn_parser* parser) {
   parser->pos = 0;
   parser->toknext = 0;
   parser->toksuper = -1;
}

static inline jsmntok_t* jsmn_alloc_token(jsmn_parser* parser, jsmntok_t* tokens, size_t num_tokens) {
   if (parser->toknext >= (unsigned int)num_tokens)
      return NULL;
   jsmntok_t* tok = &tokens[parser->toknext++];
   tok->start = tok->end = -1;
   tok->size = 0;
   tok->parent = -1;
   return tok;
}

static inline void jsmn_fill_token(jsmntok_t* token, jsmntype_t type, int start, int end) {
   token->type = type;
   token->start = start;
   token->end = end;
   token->size = 0;
}

static inline int jsmn_parse_primitive(jsmn_parser* parser, const char* js, size_t len, jsmntok_t* tokens, size_t num_tokens) {
   jsmntok_t* token;
   unsigned int start = parser->pos;

   for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      char c = js[parser->pos];
      if (c == '\t' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ']' || c == '}') {
         goto found;
      }
      if (c < 32 || c >= 127) {
         parser->pos = start;
         return JSMN_ERROR_INVAL;
      }
   }
   parser->pos = start;
   return JSMN_ERROR_PART;

found:
   token = jsmn_alloc_token(parser, tokens, num_tokens);
   if (!token) {
      parser->pos = start;
      return JSMN_ERROR_NOMEM;
   }
   jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
   token->parent = parser->toksuper;
   return 0;
}

static inline int jsmn_parse_string(jsmn_parser* parser, const char* js, size_t len, jsmntok_t* tokens, size_t num_tokens) {
   jsmntok_t* token;
   unsigned int start = parser->pos;

   parser->pos++;

   for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      char c = js[parser->pos];
      if (c == '\"') {
         token = jsmn_alloc_token(parser, tokens, num_tokens);
         if (!token) {
            parser->pos = start;
            return JSMN_ERROR_NOMEM;
         }
         jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
         token->parent = parser->toksuper;
         return 0;
      }
      if (c == '\\' && parser->pos + 1 < len) {
         parser->pos++;
         switch (js[parser->pos]) {
         case '\"': case '/': case '\\': case 'b':
         case 'f':  case 'r': case 'n':  case 't':
            break;
         case 'u':
            parser->pos++;
            for (int i = 0; i < 4 && parser->pos < len; i++, parser->pos++) {
               char h = js[parser->pos];
               if (!((h >= '0' && h <= '9') || (h >= 'A' && h <= 'F') || (h >= 'a' && h <= 'f'))) {
                  parser->pos = start;
                  return JSMN_ERROR_INVAL;
               }
            }
            parser->pos--;
            break;
         default:
            parser->pos = start;
            return JSMN_ERROR_INVAL;
         }
      }
   }
   parser->pos = start;
   return JSMN_ERROR_PART;
}

static inline int jsmn_parse(jsmn_parser* parser, const char* js, size_t len, jsmntok_t* tokens, unsigned int num_tokens) {
   int r;
   int i;
   jsmntok_t* token;
   int count = (int)parser->toknext;

   for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      char c = js[parser->pos];
      jsmntype_t type;

      switch (c) {
      case '{': case '[':
         count++;
         token = jsmn_alloc_token(parser, tokens, num_tokens);
         if (!token)
            return JSMN_ERROR_NOMEM;
         if (parser->toksuper != -1 && tokens != NULL)
            tokens[parser->toksuper].size++;
         token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
         token->start = parser->pos;
         token->parent = parser->toksuper;
         parser->toksuper = (int)parser->toknext - 1;
         break;

      case '}': case ']':
         type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
         if (tokens == NULL)
            break;
         for (i = (int)parser->toknext - 1; i >= 0; i--) {
            token = &tokens[i];
            if (token->start != -1 && token->end == -1) {
               if (token->type != type)
                  return JSMN_ERROR_INVAL;
               parser->toksuper = token->parent;
               token->end = parser->pos + 1;
               break;
            }
         }
         if (i == -1)
            return JSMN_ERROR_INVAL;
         for (; i >= 0; i--) {
            token = &tokens[i];
            if (token->start != -1 && token->end == -1) {
               parser->toksuper = i;
               break;
            }
         }
         if (i == -1)
            parser->toksuper = -1;
         break;

      case '\"':
         r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
         if (r < 0)
            return r;
         count++;
         if (parser->toksuper != -1 && tokens != NULL)
            tokens[parser->toksuper].size++;
         break;

      case '\t': case '\r': case '\n': case ' ':
         break;

      case ':':
         parser->toksuper = (int)parser->toknext - 1;
         break;

      case ',':
         if (tokens != NULL && parser->toksuper != -1 &&
             tokens[parser->toksuper].type != JSMN_ARRAY &&
             tokens[parser->toksuper].type != JSMN_OBJECT) {
            parser->toksuper = tokens[parser->toksuper].parent;
         }
         break;

      default:
         r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
         if (r < 0)
            return r;
         count++;
         if (parser->toksuper != -1 && tokens != NULL)
            tokens[parser->toksuper].size++;
         break;
      }
   }

   if (tokens != NULL) {
      for (i = (int)parser->toknext - 1; i >= 0; i--) {
         if (tokens[i].start != -1 && tokens[i].end == -1)
            return JSMN_ERROR_PART;
      }
   }

   return count;
}

/* Helper: compare JSON string token to a C string */
static inline int jsmn_streq(const char* json, const jsmntok_t* tok, const char* s) {
   if (tok->type == JSMN_STRING) {
      size_t len = strlen(s);
      if ((size_t)(tok->end - tok->start) == len &&
          memcmp(json + tok->start, s, len) == 0)
         return 1;
   }
   return 0;
}

/* Helper: copy JSON string token content into a buffer */
static inline void jsmn_strcpy(const char* json, const jsmntok_t* tok, char* buf, size_t bufsz) {
   size_t len = (size_t)(tok->end - tok->start);
   if (len >= bufsz)
      len = bufsz - 1;
   memcpy(buf, json + tok->start, len);
   buf[len] = '\0';
}

/* Helper: parse JSON number token as unsigned long long */
static inline unsigned long long jsmn_ull(const char* json, const jsmntok_t* tok) {
   char buf[32];
   jsmn_strcpy(json, tok, buf, sizeof(buf));
   unsigned long long v = 0;
   for (int i = 0; buf[i] >= '0' && buf[i] <= '9'; i++)
      v = v * 10 + (unsigned long long)(buf[i] - '0');
   return v;
}

/* Helper: count tokens consumed by the subtree rooted at index idx */
static inline int jsmn_subtree_size(const jsmntok_t* tokens, int idx) {
   int count = 1;
   if (tokens[idx].type == JSMN_OBJECT) {
      for (int i = 0; i < tokens[idx].size; i++) {
         count += jsmn_subtree_size(tokens, idx + count); /* key */
         count += jsmn_subtree_size(tokens, idx + count); /* value */
      }
   } else if (tokens[idx].type == JSMN_ARRAY) {
      for (int i = 0; i < tokens[idx].size; i++) {
         count += jsmn_subtree_size(tokens, idx + count);
      }
   }
   return count;
}

#endif /* HEADER_jsmn */
