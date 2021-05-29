int api_open_win(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_boxfill_win(int win, int x0, int y0, int x1, int y1, int col);
void api_malloc_init(void);
void *api_malloc(int size);
void api_point(int win, int x, int y, int col);
void api_end(void);

int main(void) {
  api_malloc_init();
  char *buf = api_malloc(150 * 100);
  int win = api_open_win(buf, 150, 100, -1, "star1");
  api_boxfill_win(win, 6, 26, 143, 93, 0);
  api_point(win, 75, 59, 3);

  api_end();

  return 0;
}
