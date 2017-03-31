
/* Released under the 2-Clause BSD License. */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "stream.h"

lu_stream_t *lu_stream_create(const size_t capacity) {

  lu_stream_t *s = malloc(sizeof(lu_stream_t));
  if (!s)
    return NULL;

  s->buf = malloc(capacity * sizeof(char));
  if (!s->buf) {
    free(s);
    return NULL;
  }

  s->size = 0;
  s->capacity = capacity;
  s->head = s->buf;
  s->tail = s->buf;
  s->end = s->buf + capacity;
  return s;
}

int lu_stream_destroy(lu_stream_t *s) {

  if (s) {
    if (s->buf)
      free(s->buf);
    free(s);
    return 0;
  }
  return -1;
}

int lu_stream_push(lu_stream_t *s, const char data) {

  if (s->size == s->capacity)
    return -1;
  *s->head = data;
  s->size++;
  s->head++;
  if (s->head == s->end)
    s->head = s->buf;
  return 0;
}

int lu_stream_pop(lu_stream_t *s, char *data) {

  if (s->size == 0)
    return -1;
  *data = *s->tail;
  s->size--;
  s->tail++;
  if (s->tail == s->end)
    s->tail = s->buf;
  return 0;
}

int lu_stream_readin_fd(lu_stream_t *s, const int fd) {

  int r, t;
  size_t ss;

  if (s == NULL)
    return LU_NULL;

  do {

    t = s->tail - s->head;
    ss = t > 0 ? t : (s->end - s->head);
    if (ss == 0) {
      /* Buffer full. */
      break;
    }

    r = read(fd, s->head, ss);
    if (r < 0) {
      if (errno == EINTR)
        continue;
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      return -1;
    }

    s->size += (size_t)r;
    s->head += (size_t)r;

    if (s->head == s->end)
      s->head = s->buf;
    if (s->head == s->tail)
      break;

  } while (r > 0);

  return 0;
}

int lu_stream_readline(lu_stream_t *s, char *buf, const size_t buflen) {

  char *t, *r = buf;
  size_t p, b = buflen;

  if (s == NULL)
    return LU_NULL;

  p = s->size;
  t = s->tail;

  do {

    if (*t == '\n') {
      *r = 0;
      t++;
      p--;
      s->tail = (t == s->end) ? s->buf : t;
      s->size = p;
      return 0;
    }

    *r++ = *t++;
    p--;
    if (!--b)
      return LU_FULL;

    if (t == s->end)
      t = s->buf;

  } while (p > 0);

  return LU_EMPTY;
}
