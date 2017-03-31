
/* Released under the 2-Clause BSD License. */

#ifndef LU_SCHEDULER_H
#define LU_SCHEDULER_H

#include <sys/time.h>

/* TASKS */

typedef struct {
  struct timeval time;
  void (*cb)(void *);
  void *data;
} task_t;

typedef struct {
  task_t *tasks;
  int size;
  int last;
} heap_t;

/* FD */

enum fd_opts {
  LS_NONE = 0x0,
  LS_READ = 0x1,
  LS_WRITE = 0x2,
  LS_BOTH = 0x3
};

typedef struct lu_fdn lu_fdn_t;

struct lu_fdn {
  int fd;
  unsigned int options;
  void (*recv)(void *);
  void (*send)(void *);
  void *data;
  lu_fdn_t *next;
  lu_fdn_t *prev;
};

int lu_init(void);
int lu_loop(void);

int lu_task_insert(time_t s, void (*)(void *), void *);
int lu_task_remove(void *);

lu_fdn_t *lu_fd_add(lu_fdn_t *);
int lu_fd_del(lu_fdn_t *);

#endif /* LU_SCHEDULER_H */
