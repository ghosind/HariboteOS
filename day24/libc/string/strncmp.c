#include <stddef.h>
#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n) {
  for (size_t i = 0; i < n; i++, s1++, s2++) {
    if (*s1 != *s2) {
      return *s1 - *s2;
    }

    if (*s1 == '\0') {
      return 0;
    }
  }

  return 0;
}
