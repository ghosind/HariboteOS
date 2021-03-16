#include <stdio.h>

#include "bootpack.h"
#include "desctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *)ADR_BOOTINFO;
  char s[40], mcursor[256];
  unsigned char keybuf[32], key;
  // int i;

  init_gdtidt();
  init_pic(); // GDT/IDT完成初始化，开放CPU中断

  io_sti();

  fifo8_init(&keyfifo, 32, keybuf);

  init_palette();
  init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

  int mx = (binfo->scrnx - 16) / 2;
  int my = (binfo->scrny - 28 - 16) / 2;
  init_mouse_cursor8(mcursor, COL8_008484);
  put_block8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
  sprintf(s, "(%d, %d)", mx, my);
  put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

  io_out8(PIC0_IMR, 0xf9); // 开放PIC1以及键盘中断
  io_out8(PIC1_IMR, 0xef); // 开放鼠标中断

  for (;;) {
    io_cli();
    if (!fifo8_status(&keyfifo)) {
      io_stihlt();
    } else {
      key = (unsigned char)fifo8_get(&keyfifo);

      io_sti();
      sprintf(s, "%02X", key);
      box_fill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
      put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
    }
  }

  return 0;
}
