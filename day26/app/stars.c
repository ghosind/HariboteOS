#include <stdlib.h>

int api_open_win(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_boxfill_win(int win, int x0, int y0, int x1, int y1, int col);
void api_malloc_init(void);
void *api_malloc(int size);
void api_point(int win, int x, int y, int col);
void api_end(void);

int main(void) {
  api_malloc_init();
  char *buf = api_malloc(150 * 100);
  int win = api_open_win(buf, 150, 100, -1, "stars");
  api_boxfill_win(win, 6, 26, 143, 93, 0);
  
  for (int i = 0; i < 50; i++) {
    int x = (rand() % 137) + 6;
    int y = (rand() % 67) + 6;
    api_point(win, x, y, 3);
  }

  api_end();

  return 0;
}
