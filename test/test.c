#include "test.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned int ok = 0;
static unsigned int fail = 0;

static void test_ok(void)
{
  printf("[ " ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET " ] ");
  ok++;
}

void test_fail(void)
{
  printf("[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "] ");
  fail++;
}

void test_title(char *title)
{
  printf("--------\n" ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n--------\n", title);
}

void test_stats(void)
{
  printf("OK: %u, FAIL: %u\n", ok, fail);
}

int assert_u32(char *text, uint32_t value, uint32_t expected)
{
  if (value != expected) {
    test_fail();
    printf("U32 %s: value %u, expected %u\n", text, value, expected);
    return -1;
  }
  test_ok();
  printf("U32 %s\n", text);
  return 0;
}

int assert_u16(char *text, uint16_t value, uint16_t expected)
{
  if (value != expected) {
    test_fail();
    printf("U16 %s: value %u, expected %u\n", text, value, expected);
    return -1;
  }
  test_ok();
  printf("U16 %s\n", text);
  return 0;
}

int assert_u8(char *text, uint8_t value, uint8_t expected)
{
  if (value != expected) {
    test_fail();
    printf("U8 %s: value %u, expected %u\n", text, value, expected);
    return -1;
  }
  test_ok();
  printf("U8 %s: value %u\n", text, value);
  return 0;
}

int assert_uint(const char *text, const unsigned int value, const unsigned int expected)
{
  if (value != expected) {
    test_fail();
    printf("UINT %s: value %u, expected %u\n", text, value, expected);
    return -1;
  }
  test_ok();
  printf("UINT %s: value %u\n", text, value);
  return 0;
}

int assert_string(const char *text, const char *value, const char *expected)
{
  unsigned int expected_len;
  unsigned int len;
  if (!value) {
    if (!expected) {
      test_ok();
      printf("STRING %s: value NULL\n", text);
      return 0;
    }
    test_fail();
    printf("STRING %s: value NULL, expected %s\n", text, expected);
    return -1;
  }
  expected_len = strlen(expected);
  len = strnlen(value, expected_len + 1);
  if (len < expected_len || *(value + expected_len) != 0) {
    test_fail();
    printf("STRING %s: value %s, expected %s\n", text, value, expected);
    return -1;
  }
  if (strncmp(value, expected, expected_len)) {
    test_fail();
    printf("STRING %s: value %s, expected %s\n", text, value, expected);
    return -1;
  }
  test_ok();
  printf("STRING %s: value %s\n", text, value);
  return 0;
}
