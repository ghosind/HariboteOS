int api_open_win(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_end(void);

char buf[150 * 50];

int main(void) {
  int win = api_open_win(buf, 150, 50, -1, "hello");
  api_end();

  return 0;
}
