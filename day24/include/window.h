#include "sheet.h"

#ifndef _WINDOW_H_
#define _WINDOW_H_

void make_window8(unsigned char *buf, int xsize, int ysize, char *title,
                  char act);
void make_textbox8(struct Sheet *sht, int x0, int y0, int sx, int sy, int c);
void make_window_title8(unsigned char *buf, int xsize, char *title, char act);
void change_window_title8(struct Sheet *sht, char act);

int keywin_off(struct Sheet *key_win, struct Sheet *sht_win, int cur_c,
               int cur_x);
int keywin_on(struct Sheet *key_win, struct Sheet *sht_win, int cur_c);

#endif // _WINDOW_H_
