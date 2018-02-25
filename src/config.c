
/* Released under the 2-Clause BSD License. */

#include <string.h>
#include "config.h"

static char *lu_cfgarg = NULL;

char lu_getopt(const lu_args *args, const char *opts)
{
  char *p, *q, *r;
  static int i = 1;

  if (i == args->argc) {
    i = 1;
    return EOF;
  }

  lu_cfgarg = NULL;

  p = *(args->argv + i);
  i++;
  if (*p != '-') {
    lu_cfgarg = p;
    return '_';
  }
  p++;
  if (!*p) {
    return '?';
  }
  if (*p == '-') {
    /* -- */
    i = 1;
    return EOF;
  }

  /* Find opts letter: q. */
  q = p;
  r = strchr(opts, *q);
  if (!r || *q == ':') {
    return '?';
  }

  r++;
  if (*r == ':') {
    p++;
    if (*p) {
      /* -parg */
      lu_cfgarg = p;
    }
    else if (i < args->argc) {
      /* -p arg */
      lu_cfgarg = *(args->argv + i);
      i++;
    }
    else {
      return ':';
    }
  }

  return *q;
}

const char *lu_getarg(void)
{
  return lu_cfgarg;
}
