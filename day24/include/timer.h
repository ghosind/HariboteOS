#include "fifo.h"

#ifndef _TIMER_H_
#define _TIMER_H_

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

#define TIMER_FLAGS_ALLOC 1 // 已配置状态
#define TIMER_FLAGS_USING 2 // 定时器运行中

#define MAX_TIMER 500

struct Timer {
  struct Timer *next;
  unsigned int flags, flags2;
  unsigned int timeout;
  struct FIFO32 *fifo;
  int data;
};

struct TimerCtl {
  unsigned int count, next;
  struct Timer *t0;
  struct Timer timers0[MAX_TIMER];
};

extern struct TimerCtl timerctl;

void init_pit(void);
struct Timer *timer_alloc(void);
void timer_free(struct Timer *timer);
void timer_init(struct Timer *timer, struct FIFO32 *fifo, int data);
void timer_set_timer(struct Timer *timer, unsigned int timeout);
int timer_cancel(struct Timer *timer);
void timer_cancel_all(struct FIFO32 *fifo);

void int_handler20(int *esp);

#endif // _TIMER_H_
