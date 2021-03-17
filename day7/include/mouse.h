#include "fifo.h"

#ifndef _MOUSE_H_
#define _MOUSE_H_

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

#define MOUSE_FIFO_BUF_SIZE 128

extern struct FIFO8 mousefifo;
extern unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

void enable_mouse(void);

#endif // _MOUSE_H_
