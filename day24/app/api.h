#ifndef U_API_H
#define U_API_H

void api_end(void);

void api_putchar(int c);
void api_putstr(char *s);

int api_open_win(char *buf, int xsize, int ysize, int col_inv, char *title);
void api_close_win(int win);

void api_putstr_win(int win, int x, int y, int col, int len, char *str);
void api_boxfill_win(int win, int x0, int y0, int x1, int y1, int col);

void api_malloc_init(void);
void *api_malloc(int size);
void api_free(void *addr, int size);

void api_point(int win, int x, int y, int col);
void api_line_win(int win, int x0, int y0, int x1, int y1, int col);
void api_refresh_win(int win, int x0, int y0, int x1, int y1);

int api_get_key(int mode);

#endif // U_API_H
