
/* Released under the 2-Clause BSD License. */

#ifndef LU_CONFIG_H
#define LU_CONFIG_H

typedef struct {
  int argc;
  char **argv;
} lu_args;

char lu_getopt(const lu_args *, const char *);
const char *lu_getarg(void);

#ifndef EOF
# define EOF -1
#endif

#endif /* LU_CONFIG_H */
