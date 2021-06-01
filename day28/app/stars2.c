#include <stdlib.h>

#include "api.h"

int main() {
  api_malloc_init();

  char *buf = api_malloc(150 * 100);
  int win = api_open_win(buf, 150, 100, -1, "stars2");
  api_boxfill_win(win + 1, 6, 26, 143, 93, 0);

  for (int i = 0; i < 50; i++) {
    int x = (rand() % 137) + 6;
    int y = (rand() % 67) + 26;
    api_point(win + 1, x, y, 3);
  }

  api_refresh_win(win, 6, 26, 144, 94);

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }
  
  api_end();

  return 0;
}
