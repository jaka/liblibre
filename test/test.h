#ifndef _TEST_H
#define _TEST_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void test_title(char *);
void test_fail(void);

int assert_uint(const char *, const unsigned int, const unsigned int);
int assert_string(const char *, const char *, const char *);

#endif
