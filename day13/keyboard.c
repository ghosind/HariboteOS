#include "keyboard.h"
#include "fifo.h"
#include "int.h"
#include "io.h"

struct FIFO32 *keyfifo;
int keydata0;

void wait_KBC_sendready(void) {
  for (;;) {
    if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
      break;
    }
  }
}

void init_keyboard(struct FIFO32 *fifo, int data0) {
  keyfifo = fifo;
  keydata0 = data0;

  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, KBC_MODE);
}

void int_handler21(int *esp) {
  io_out8(PIC0_OCW2, 0x61); // 通知PIC IRQ-1的受理已经完成
  int data = io_in8(PORT_KEYDAT);

  fifo32_put(keyfifo, data + keydata0);
}
