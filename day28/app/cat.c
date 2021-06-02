#include "api.h"

int main() {
  char c, cmdline[30], *p;

  api_cmdline(cmdline, 30);

  for (p = cmdline; *p > ' '; p++) {
  }
  for (; *p == ' '; p++) {
  }

  int fh = api_fopen(p);
  if (fh) {
    for (;;) {
      if (api_fread(&c, 1, fh) == 0) {
        break;
      }

      api_putchar(c);
    }
  } else {
    api_putstr("File not found.\n");
  }

  api_end();

  return 0;
}
