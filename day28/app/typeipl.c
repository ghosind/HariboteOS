#include "api.h"

int main() {
  int fh = api_fopen("ipl.asm");
  char c;

  if (fh) {
    for (;;) {
      if (api_fread(&c, 1, fh) == 0) {
        break;
      }

      api_putchar(c);
    }
  }

  api_end();

  return 0;
}
