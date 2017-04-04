# liblibre

### scheduler.h

Scheduler main function `lu_loop` executes a callback function when a specific event occurs. An event is triggered when a timeout take place or a file descriptors become ready.

Timeouts are stored in a heap and file descriptors in a double linked list.

### stream.h

This header implements a buffered writer/reader. Buffer is filled by reading a file descriptor and consumed by outputing a lines (separated by newline character `\n`).

### config.h

This provides an alternative to `getopt()` function.

#### Disclaimer
I'm learning by doing.
