#include "api.h"

char buf[150 * 50];

int main(void) {
  int win = api_open_win(buf, 150, 50, -1, "hello");

  for (;;) {
    if (api_get_key(1) == 0x0a) {
      break;
    }
  }

  api_end();

  return 0;
}
