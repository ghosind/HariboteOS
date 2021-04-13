#ifndef _FIFO_H_
#define _FIFO_H_

#include "task.h"

#define FLAGS_OVERRUN 0x0001

struct FIFO8 {
  unsigned char *buf;
  int size, free, flags;
  int next_r, next_w;
};

struct FIFO32 {
  int *buf;
  int size, free, flags;
  int next_r, next_w;
  struct Task *task;
};

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct Task *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

#endif // _FIFO_H_
