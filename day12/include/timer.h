#include "fifo.h"

#ifndef _TIMER_H_
#define _TIMER_H_

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

#define TIMER_FIFO_BUF_SIZE 8

struct TimerCtl {
  unsigned int count;
  unsigned int timeout;
  struct FIFO8 *fifo;
  unsigned char data;
};

extern struct TimerCtl timerctl;
extern struct FIFO8 timerfifo;
extern unsigned char timerbuf[TIMER_FIFO_BUF_SIZE];

void init_pit(void);
void set_timer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data);

void int_handler20(int *esp);

#endif // _TIMER_H_
