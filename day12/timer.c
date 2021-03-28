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
  for (int i = 0; i < MAX_TIMER; i++) {
    timerctl.timer[i].flags = 0;
  }
}

struct Timer *timer_alloc(void) {
  for (int i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timer[i].flags == 0) {
      timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
      return &timerctl.timer[i];
    }
  }

  return NULL;
}

void timer_free(struct Timer *timer) {
  timer->flags = 0;
}

void timer_init(struct Timer *timer, struct FIFO8 *fifo, unsigned char data) {
  timer->fifo = fifo;
  timer->data = data;
}

void timer_set_timer(struct Timer *timer, unsigned int timeout) {
  timer->timeout = timeout;
  timer->flags = TIMER_FLAGS_USING;
}

void int_handler20(int *esp) {
  io_out8(PIC0_OCW2, 0x60); // 接收IRQ-00信号通知PIC
  timerctl.count++;

  for (int i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timer[i].flags == TIMER_FLAGS_USING) {
      timerctl.timer[i].timeout--;
      if (timerctl.timer[i].timeout == 0) {
        timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
        fifo8_put(timerctl.timer[i].fifo, timerctl.timer[i].data);
      }
    }
  }
}
