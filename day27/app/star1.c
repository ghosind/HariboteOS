#include "api.h"

int main(void) {
  api_malloc_init();
  char *buf = api_malloc(150 * 100);
  int win = api_open_win(buf, 150, 100, -1, "star1");
  api_boxfill_win(win, 6, 26, 143, 93, 0);
  api_point(win, 75, 59, 3);

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }

  api_end();

  return 0;
}
