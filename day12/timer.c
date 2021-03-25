#include "int.h"
#include "io.h"
#include "timer.h"

struct TimerCtl timerctl;

void init_pit(void) {
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);
  timerctl.count = 0;
}

void int_handler20(int *esp) {
  io_out8(PIC0_OCW2, 0x60); // 接收IRQ-00信号通知PIC
  timerctl.count++;
}
