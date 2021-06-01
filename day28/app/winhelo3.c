#include "api.h"

int main(void) {
  api_malloc_init();
  char *buf = api_malloc(150 * 50);
  int win = api_open_win(buf, 150, 50, -1, "hello");
  api_boxfill_win(win, 8, 36, 141, 43, 6);
  api_putstr_win(win, 28, 28, 0, 12, "hello world");

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }

  api_end();

  return 0;
}
