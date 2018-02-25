#include "test.h"
#include "../src/config.c"

#define FOO "foo"

int test1(void)
{
  lu_args args;
  char c;

  const char *argv[] = { "a.out", "-a", FOO };
  args.argc = 3;
  args.argv = (char **)argv;
  
  while ((c = lu_getopt(&args, "a:b")) != EOF) {
    if (c == 'a') {
      assert_string("Parameter", lu_getarg(), FOO);
    }
	else {
      test_fail();
      return -1;
	}
  }
  return 0;
}

int test2(void)
{
  lu_args args;
  char c;

  const char *argv[] = { "a.out", "-a"};
  args.argc = 2;
  args.argv = (char **)argv;
  
  while ((c = lu_getopt(&args, "a:b")) != EOF) {
    if (c == ':') {
      assert_string("Parameter", lu_getarg(), NULL);
    }
	else {
      test_fail();
      return -1;
	}
  }
  return 0;
}

int test3(void)
{
  lu_args args;
  char c;

  const char *argv[] = { "a.out", "-b" FOO};
  args.argc = 2;
  args.argv = (char **)argv;
  
  while ((c = lu_getopt(&args, "b:")) != EOF) {
    if (c == 'b') {
      assert_string("Parameter", lu_getarg(), FOO);
    }
	else {
      test_fail();
      return -1;
	}
  }
  return 0;
}

int main(void)
{
  test_title("liblibre config");
  
  test1();
  test2();
  test3();
}
