#ifndef _FIFO_H_
#define _FIFO_H_

#define FLAGS_OVERRUN 0x0001

struct FIFO8 {
  unsigned char *buf;
  int size, free, flags;
  int next_r, next_w;
};

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

#endif // _FIFO_H_
