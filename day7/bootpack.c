#include <stdio.h>

#include "bootpack.h"
#include "desctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *)ADR_BOOTINFO;
  char s[40], mcursor[256];
  unsigned char key;

  init_gdtidt();
  init_pic(); // GDT/IDT完成初始化，开放CPU中断

  io_sti();

  fifo8_init(&keyfifo, KEY_FIFO_BUF_SIZE, keybuf);
  fifo8_init(&mousefifo, MOUSE_FIFO_BUF_SIZE, mousebuf);

  io_out8(PIC0_IMR, 0xf9); // 开放PIC1以及键盘中断
  io_out8(PIC1_IMR, 0xef); // 开放鼠标中断

  init_keyboard();

  init_palette();
  init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

  int mx = (binfo->scrnx - 16) / 2;
  int my = (binfo->scrny - 28 - 16) / 2;
  init_mouse_cursor8(mcursor, COL8_008484);
  put_block8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
  sprintf(s, "(%d, %d)", mx, my);
  put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

  enable_mouse();

  for (;;) {
    io_cli();
    if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
      io_stihlt();
    } else {
      if (fifo8_status(&keyfifo)) {
        key = (unsigned char)fifo8_get(&keyfifo);

        io_sti();
        sprintf(s, "%02X", key);
        box_fill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
        put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
      } else if (fifo8_status(&mousefifo)) {
        key = (unsigned char)fifo8_get(&mousefifo);

        io_sti();
        sprintf(s, "%02X", key);
        box_fill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 47, 31);
        put_fonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
      }
    }
  }

  return 0;
}
