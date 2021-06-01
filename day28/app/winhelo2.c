#include "api.h"

char buf[150 * 50];

int main(void) {
  int win = api_open_win(buf, 150, 50, -1, "hello");
  api_boxfill_win(win, 8, 36, 141, 43, 3);
  api_putstr_win(win, 28, 28, 0, 12, "hello world");

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }

  api_end();

  return 0;
}
