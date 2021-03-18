#include "fifo.h"

#ifndef _MOUSE_H_
#define _MOUSE_H_

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

#define MOUSE_FIFO_BUF_SIZE 128

struct MouseDec {
  unsigned char buf[3], phase;
  int x, y, btn;
};

extern struct FIFO8 mousefifo;
extern unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

void enable_mouse(struct MouseDec *mdec);
int mouse_decode(struct MouseDec *mdec, unsigned char dat);

#endif // _MOUSE_H_
