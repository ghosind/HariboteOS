#include "api.h"

int main() {
  char a[100];

  a[10] = 'A';
  api_putchar(a[10]);

  a[102] = 'B';
  api_putchar(a[102]);

  a[123] = 'C';
  api_putchar(a[123]);

  api_end();

  return 0;
}
