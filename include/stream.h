
/* Released under the 2-Clause BSD License. */

#ifndef LU_STREAM_H
#define LU_STREAM_H

#define LU_ERROR -1

#define LU_EMPTY -2
#define LU_FULL -2
#define LU_NULL -3

typedef struct {
  char *buf;
  size_t size;
  size_t capacity;
  char *head;
  char *tail;
  const char *end;
} lu_stream_t;

lu_stream_t *lu_stream_create(const size_t);
int lu_stream_destroy(lu_stream_t *);

int lu_stream_readin_fd(lu_stream_t *, const int);
int lu_stream_readline(lu_stream_t *, char *, const size_t);

#endif /* LU_STREAM_H */
