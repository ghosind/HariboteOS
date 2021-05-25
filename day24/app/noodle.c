#include <stdio.h>

#include "api.h"

int main() {
  char s[12];
  int hour = 0, min = 0, sec = 0;

  api_malloc_init();

  char *buf = api_malloc(150 * 50);
  int win = api_open_win(buf, 150, 50, -1, "noodle");
  int timer = api_alloc_timer();
  
  api_init_timer(timer, 128);

  for (;;) {
    sprintf(s, "%5d:%02d:%02d", hour, min, sec);

    api_boxfill_win(win, 28, 27, 115, 41, 7);
    api_putstr_win(win, 28, 27, 0, 11, s);

    api_set_timer(timer, 100);
    if (api_get_key(1) != 128) {
      break;
    }

    sec++;
    if (sec == 60) {
      sec = 0;
      min++;
      if (min == 60) {
        min = 0;
        hour++;
      }
    }
  }

  api_end();

  return 0;
}
