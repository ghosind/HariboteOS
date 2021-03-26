#include "int.h"
#include "io.h"
#include "timer.h"

struct TimerCtl timerctl;

struct FIFO8 timerfifo;

unsigned char timerbuf[TIMER_FIFO_BUF_SIZE];

void init_pit(void) {
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);
  timerctl.count = 0;
  timerctl.timeout = 0;
}

void set_timer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data) {
  int eflags;

  eflags = io_load_eflags();
  io_cli();

  timerctl.timeout = timeout;
  timerctl.fifo = fifo;
  timerctl.data = data;

  io_store_eflags(eflags);
}

void int_handler20(int *esp) {
  io_out8(PIC0_OCW2, 0x60); // 接收IRQ-00信号通知PIC
  timerctl.count++;

  if (timerctl.timeout > 0) {
    // 如果设置了超时
    timerctl.timeout--;
    if (timerctl.timeout == 0) {
      fifo8_put(timerctl.fifo, timerctl.data);
    }
  }
}
