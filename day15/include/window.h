#include "sheet.h"

#ifndef _WINDOW_H_
#define _WINDOW_H_

void make_window8(unsigned char *buf, int xsize, int ysize, char *title);
void make_textbox8(struct Sheet *sht, int x0, int y0, int sx, int sy, int c);

#endif // _WINDOW_H_
