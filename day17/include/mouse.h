#include "fifo.h"

#ifndef _MOUSE_H_
#define _MOUSE_H_

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

struct MouseDec {
  unsigned char buf[3], phase;
  int x, y, btn;
};

void enable_mouse(struct FIFO32 *fifo, int data0, struct MouseDec *mdec);
int mouse_decode(struct MouseDec *mdec, unsigned char dat);

void int_handler27(int *esp);

#endif // _MOUSE_H_
