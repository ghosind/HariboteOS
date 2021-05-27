#include "api.h"

int main() {
  api_malloc_init();

  char *buf = api_malloc(160 * 100);
  int win = api_open_win(buf, 160, 100, -1, "walk");

  api_boxfill_win(win, 4, 24, 155, 95, 0);

  int x = 76;
  int y = 56;

  api_putstr_win(win, x, y, 3, 1, "*");

  for (;;) {
    int data = api_get_key(1);
    api_putstr_win(win, x, y, 0, 1, "*");

    if (data == '4' && x > 4) {
      x -= 8;
    }
    if (data == '6' && x < 148) {
      x += 8;
    }
    if (data == '8' && y > 24) {
      y -= 8;
    }
    if (data == '2' && y < 80) {
      y += 8;
    }

    if (data == 0x0a) {
      break;
    }

    api_putstr_win(win, x, y, 3, 1, "*");
  }

  api_close_win(win);
  api_end();

  return 0;
}
