int api_open_win(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_putstr_win(int win, int x, int y, int col, int len, char *str);
void api_boxfill_win(int win, int x0, int y0, int x1, int y1, int col);
void api_malloc_init(void);
void *api_malloc(int size);
void api_end(void);

int main(void) {
  api_malloc_init();
  char *buf = api_malloc(150 * 50);
  int win = api_open_win(buf, 150, 50, -1, "hello");
  api_boxfill_win(win, 8, 36, 141, 43, 6);
  api_putstr_win(win, 28, 28, 0, 12, "hello world");

  api_end();

  return 0;
}
