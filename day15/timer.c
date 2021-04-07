#include <stdio.h>

#include "int.h"
#include "io.h"
#include "timer.h"

struct TimerCtl timerctl;

void init_pit(void) {
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);

  for (int i = 0; i < MAX_TIMER; i++) {
    timerctl.timers0[i].flags = 0;
  }

  struct Timer *t = timer_alloc();
  t->timeout = 0xffffffff;
  t->flags = TIMER_FLAGS_USING;
  t->next = NULL;

  timerctl.count = 0;
  timerctl.t0 = t;
  timerctl.next = 0xffffffff;
}

struct Timer *timer_alloc(void) {
  for (int i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timers0[i].flags == 0) {
      timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
      return &timerctl.timers0[i];
    }
  }

  return NULL;
}

void timer_free(struct Timer *timer) { timer->flags = 0; }

void timer_init(struct Timer *timer, struct FIFO32 *fifo, int data) {
  timer->fifo = fifo;
  timer->data = data;
}

void timer_set_timer(struct Timer *timer, unsigned int timeout) {
  int eflags;
  struct Timer *t, *s;

  timer->timeout = timeout + timerctl.count;
  timer->flags = TIMER_FLAGS_USING;

  eflags = io_load_eflags();
  io_cli();

  t = timerctl.t0;
  if (timer->timeout <= t->timeout) {
    timerctl.t0 = timer;
    timer->next = t;
    timerctl.next = timer->timeout;
    io_store_eflags(eflags);
    return;
  }

  // 搜索插入未知
  for (;;) {
    s = t;
    t = t->next;

    if (timer->timeout <= t->timeout) {
      s->next = timer;
      timer->next = t;
      io_store_eflags(eflags);
      return;
    }
  }
}

void int_handler20(int *esp) {
  io_out8(PIC0_OCW2, 0x60); // 接收IRQ-00信号通知PIC
  timerctl.count++;

  if (timerctl.next > timerctl.count) {
    return;
  }

  struct Timer *timer = timerctl.t0;
  for (;;) {
    // timers的定时器都处于动作中，所以不确认flags
    if (timer->timeout > timerctl.count) {
      break;
    }

    // 超时
    timer->flags = TIMER_FLAGS_ALLOC;
    fifo32_put(timer->fifo, timer->data);
    timer = timer->next;
  }

  timerctl.t0 = timer;
  timerctl.next = timer->timeout;
}
