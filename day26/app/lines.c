#include "api.h"

int main() {
  api_malloc_init();

  char *buf = api_malloc(160 * 100);
  int win = api_open_win(buf, 160, 100, -1, "lines");

  for (int i = 0; i < 8; i++) {
    api_line_win(win + 1, 8, 26, 77, i * 9 + 26, i);
    api_line_win(win + 1, 88, 26, i * 9 + 88, 89, i);
  }

  api_refresh_win(win, 6, 26, 154, 90);

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }

  api_close_win(win);
  api_end();

  return 0;
}
