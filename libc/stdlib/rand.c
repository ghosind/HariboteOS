#include <stdlib.h>

static unsigned long int next = 1;

/**
 * rand() implementation from The C Programming Language.
 */
int rand(void) {
  next = next * 1103515245 + 12345;

  return (unsigned int)(next / 65536) % 32768;
}
