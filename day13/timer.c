#include <stdio.h>

#include "int.h"
#include "io.h"
#include "timer.h"

struct TimerCtl timerctl;

void init_pit(void) {
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);
  timerctl.count = 0;
  timerctl.next = 0xffffffff;
  timerctl.using = 0;
  for (int i = 0; i < MAX_TIMER; i++) {
    timerctl.timers0[i].flags = 0;
  }
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

void timer_free(struct Timer *timer) {
  timer->flags = 0;
}

void timer_init(struct Timer *timer, struct FIFO32 *fifo, int data) {
  timer->fifo = fifo;
  timer->data = data;
}

void timer_set_timer(struct Timer *timer, unsigned int timeout) {
  int eflags, i, j;

  timer->timeout = timeout + timerctl.count;
  timer->flags = TIMER_FLAGS_USING;

  eflags = io_load_eflags();
  io_cli();

  for (i = 0; i < timerctl.using; i++) {
    if (timerctl.timers[i]->timeout >= timer->timeout) {
      break;
    }
  }

  // i后全部后移一位
  for (j = timerctl.using; j > i; j--) {
    timerctl.timers[j] = timerctl.timers[j - 1];
  }

  timerctl.using++;
  // 插入到空位上
  timerctl.timers[i] = timer;
  timerctl.next = timerctl.timers[0]->timeout;
  
  io_store_eflags(eflags);
}

void int_handler20(int *esp) {
  int i, j;

  io_out8(PIC0_OCW2, 0x60); // 接收IRQ-00信号通知PIC
  timerctl.count++;

  if (timerctl.next > timerctl.count) {
    return;
  }

  for (i = 0; i < timerctl.using; i++) {
    // timers的定时器都处于动作中，所以不确认flags
    if (timerctl.timers[i]->timeout > timerctl.count) {
      break;
    }

    // 超时
    timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
    fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
  }

  // 正好有i个定时器超时了，其余的进行移位
  timerctl.using -= i;
  for (j = 0; j < timerctl.using; j++) {
    timerctl.timers[j] = timerctl.timers[i + j];
  }

  if (timerctl.using > 0) {
    timerctl.next = timerctl.timers[0]->timeout;
  } else {
    timerctl.next = 0xffffffff;
  }
}
