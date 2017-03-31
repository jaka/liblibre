
/* Released under the 2-Clause BSD License. */

#define _POSIX_C_SOURCE 199309L

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scheduler.h"

static int cur_time(struct timeval *now) {

  struct timespec cur_time;
  if (clock_gettime(CLOCK_MONOTONIC, &cur_time) < 0)
    return -1;

  now->tv_sec = cur_time.tv_sec;
  now->tv_usec = (cur_time.tv_nsec / 1000) + 1;

  return 0;
}

static int lu_task_cmp(task_t *task_a, task_t *task_b) {

  struct timeval a = task_a->time, b = task_b->time;

  return (a.tv_sec < b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec < b.tv_usec));
}

static int lu_task_swim(heap_t *h, int i) {

  int p;
  task_t task = *(h->tasks + i);

  for (; i > 0; i = p) {
    p = (i - 1) / 2;
    if (lu_task_cmp(h->tasks + p, &task) == 1)
      break;
    *(h->tasks + i) = *(h->tasks + p);
  }
  *(h->tasks + i) = task;

  return i;
}

static int lu_task_sink(heap_t *h, int i) {

  int n;
  task_t task = *(h->tasks + i);

  for (; 2 * i < h->last; i = n) {
    n = 2 * i + 1;
    if (n < (h->last - 1) && lu_task_cmp(h->tasks + n + 1, h->tasks + n) == 1)
      n++;
    if (lu_task_cmp(&task, h->tasks + n) == 1)
      break;
    *(h->tasks + i) = *(h->tasks + n);
  }
  *(h->tasks + i) = task;

  return i;
}

static int lu_task_del(heap_t *h, int i) {

  if (i >= h->last)
    return -1;

  h->last--;
  *(h->tasks + i) = *(h->tasks + h->last);

  return lu_task_sink(h, i);
}

static int lu_task_push(heap_t *h, task_t task) {

  if (h->last == h->size) {
    h->size = h->size ? h->size * 2 : 4;
    h->tasks = (task_t *)realloc(h->tasks, h->size * sizeof(task_t));
    if (h->tasks == NULL)
      return -1;
  }
  /* Append to the end of array. */
  *(h->tasks + h->last) = task;
  h->last++;

  return lu_task_swim(h, h->last - 1);
}

static int lu_task_timeout(heap_t *h, struct timeval *timeout) {

  struct timeval first, now, diff;

  if (h->last == 0)
    return -1;

  if (cur_time(&now) < 0)
    return -1;
  first = h->tasks->time;

  /* Return 1 if first is before now, otherwise calculate the difference diff = first - now. */

  if (first.tv_sec < now.tv_sec)
    return 1;

  if (first.tv_sec == now.tv_sec) {
    if (first.tv_usec <= now.tv_usec)
      return 1;
    diff.tv_sec = 0;
    diff.tv_usec = first.tv_usec - now.tv_usec;
  }
  else {
    diff.tv_sec = first.tv_sec - now.tv_sec; /* This is always more than 1. */
    if (first.tv_usec < now.tv_usec) {
      diff.tv_sec--;
      diff.tv_usec = 1000000 + first.tv_usec - now.tv_usec;
    }
    else
      diff.tv_usec = first.tv_usec - now.tv_usec;
  }

  *timeout = diff;
  return 0;
}

static int lu_task_run(heap_t *h) {

  task_t task;

  if (h->last == 0) {
    /* This should never happen. */
    return -1;
  }

  task = *h->tasks;
  lu_task_del(h, 0);
  task.cb(task.data);

  return 0;
}

/* ********************** */
static lu_fdn_t *fdn_first = NULL;
static lu_fdn_t *fdn_last = NULL;
static heap_t *tsk_heap = NULL;

lu_fdn_t *lu_fd_add(lu_fdn_t *fdn) {

  lu_fdn_t *new_fdn;

  new_fdn = (lu_fdn_t *)malloc(sizeof(lu_fdn_t));
  if (!new_fdn)
    return NULL;

  memcpy(new_fdn, fdn, sizeof(lu_fdn_t));

  /* Put the node to the start. */
  new_fdn->prev = NULL;
  new_fdn->next = fdn_first;
  if (fdn_first)
    fdn_first->prev = new_fdn;
  else
    fdn_last = new_fdn;
  fdn_first = new_fdn;

  return new_fdn;
}

int lu_fd_del(lu_fdn_t *fdn) {

  if (!fdn)
    return -1;

  if (fdn->next)
    fdn->next->prev = fdn->prev;
  if (fdn->prev)
    fdn->prev->next = fdn->next;
  if (fdn_first == fdn)
    fdn_first = fdn->next;
  if (fdn_last == fdn)
    fdn_last = fdn->prev;

  free(fdn);

  return 0;
}

int lu_task_insert(time_t s, void (*cb)(void *), void *data) {

  task_t task;
  struct timeval now;

  if (cur_time(&now) < 0)
    return -1;

  now.tv_sec += s;

  task.time = now;
  task.cb = cb;
  task.data = data;

  return lu_task_push(tsk_heap, task);
}

int lu_task_remove(void *data) {

  int i;

  if (tsk_heap->last == -1)
    return -1;

  if ((tsk_heap->tasks + tsk_heap->last - 1)->data == data) {
    tsk_heap->last--;
    return 0;
  }

  for (i = 0; i < tsk_heap->last; i++) {
    if ((tsk_heap->tasks + i)->data == data)
      return lu_task_del(tsk_heap, i);
  }

  return -1;
}

int lu_init() {

  tsk_heap = (heap_t *)calloc(1, sizeof(heap_t));

  return 0;
}

int lu_loop() {

  struct timeval task_timeout;
  struct timeval *timeout;
  fd_set readfds;
  fd_set writefds;
  lu_fdn_t *fdn, *nextfdn;
  int max_fd;
  int select_rv;
  int timeout_rv;

  for (;;) {

    if (tsk_heap->last == 0)
      break;

    timeout_rv = lu_task_timeout(tsk_heap, &task_timeout);
    switch (timeout_rv) {
      case -1:
        timeout = NULL;
        break;
      case 0:
        timeout = &task_timeout;
        break;
      case 1:
        lu_task_run(tsk_heap);
        continue;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    max_fd = 0;

#define SET_MAX_FD(fd) {if (max_fd < fd) max_fd = fd;}

    for (fdn = fdn_first; fdn; fdn = fdn->next) {
      if (fdn->options & LS_WRITE) {
        FD_SET(fdn->fd, &writefds);
        SET_MAX_FD(fdn->fd)
      }
      if (fdn->options & LS_READ) {
        FD_SET(fdn->fd, &readfds);
        SET_MAX_FD(fdn->fd)
      }
    }

#undef SET_MAX_FD

    errno = 0;
    select_rv = select(max_fd + 1, &readfds, &writefds, NULL, timeout);

    if (select_rv < -1) {
      if (errno == EINTR)
        continue;
    }
    else if (select_rv == 0) {
      /* Timeout detected, run scheduled task. */
      lu_task_run(tsk_heap);
      continue;
    }

    /* Find available fds. */
    for (fdn = fdn_first; fdn && (nextfdn = fdn->next, 1); fdn = nextfdn) {
      if (FD_ISSET(fdn->fd, &readfds)) {
        fdn->recv(fdn->data);
        select_rv--;
      }
    }
    for (fdn = fdn_first; fdn && (nextfdn = fdn->next, 1); fdn = nextfdn) {
      if (FD_ISSET(fdn->fd, &writefds)) {
        fdn->send(fdn->data);
        select_rv--;
      }
    }

  }

  return 0;
}
