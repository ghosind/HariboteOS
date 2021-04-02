#include "fifo.h"

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf) {
  fifo->size = size;
  fifo->buf = buf;
  fifo->free = size;
  fifo->flags = 0;
  fifo->next_r = 0;
  fifo->next_w = 0;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data) {
  if (fifo->free == 0) {
    fifo->flags |= FLAGS_OVERRUN;
    return -1;
  }

  fifo->buf[fifo->next_w++] = data;
  if (fifo->next_w == fifo->size) {
    fifo->next_w = 0;
  }
  fifo->free--;
  return 0;
}

int fifo8_get(struct FIFO8 *fifo) {
  if (fifo->free == fifo->size) {
    return -1;
  }

  int data = fifo->buf[fifo->next_r++];
  if (fifo->next_r == fifo->size) {
    fifo->next_r = 0;
  }
  fifo->free++;
  return data;
}

int fifo8_status(struct FIFO8 *fifo) { return fifo->size - fifo->free; }

void fifo32_init(struct FIFO32 *fifo, int size, int *buf) {
  fifo->size = size;
  fifo->buf = buf;
  fifo->free = size;
  fifo->flags = 0;
  fifo->next_r = 0;
  fifo->next_w = 0;
}

int fifo32_put(struct FIFO32 *fifo, int data) {
  if (fifo->free == 0) {
    fifo->flags |= FLAGS_OVERRUN;
    return -1;
  }

  fifo->buf[fifo->next_w] = data;
  fifo->next_w++;
  if (fifo->next_w == fifo->size) {
    fifo->next_w = 0;
  }
  fifo->free--;

  return 0;
}

int fifo32_get(struct FIFO32 *fifo) {
  if (fifo->free == fifo->size) {
    return -1;
  }

  int data = fifo->buf[fifo->next_r];
  fifo->next_r++;
  if (fifo->next_r == fifo->size) {
    fifo->next_r = 0;
  }
  fifo->free++;

  return data;
}

int fifo32_status(struct FIFO32 *fifo) { return fifo->size - fifo->free; }
