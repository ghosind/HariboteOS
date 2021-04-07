#include <stdio.h>

#include "bootpack.h"
#include "desctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "sheet.h"
#include "timer.h"
#include "window.h"

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *)ADR_BOOTINFO;
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct MouseDec mdec;
  char s[40], mcursor[256];
  unsigned int memtotal;
  struct Shtctl *shtctl;
  struct Sheet *sht_back, *sht_mouse, *sht_win;
  unsigned char *buf_back, buf_mouse[256], *buf_win;
  struct Timer *timer, *timer2, *timer3;
  struct FIFO32 fifo;
  int fifobuf[128], data;

  init_gdtidt();
  init_pic(); // GDT/IDT完成初始化，开放CPU中断

  io_sti();

  fifo32_init(&fifo, 128, fifobuf);

  init_keyboard(&fifo, 256);
  enable_mouse(&fifo, 512, &mdec);

  init_pit();

  io_out8(PIC0_IMR, 0xf8); // 开放PIT、PIC1以及键盘中断
  io_out8(PIC1_IMR, 0xef); // 开放鼠标中断

  timer = timer_alloc();
  timer_init(timer, &fifo, 10);
  timer_set_timer(timer, 1000);
  timer2 = timer_alloc();
  timer_init(timer2, &fifo, 3);
  timer_set_timer(timer2, 300);
  timer3 = timer_alloc();
  timer_init(timer3, &fifo, 1);
  timer_set_timer(timer3, 50);

  memtotal = memtest(0x00400000, 0xbfffffff);
  memman_init(memman);
  // 书中为0x00001000 ~ 0x0009e000
  // 注1: 测试时发现会造成错误（原因未知），所以改为由0x00010000开始
  // 注2: 在640*480模式下，free该段内存后会导致屏幕画面错误
  // memman_free(memman, 0x00010000, 0x0009e000); // 0x00010000 ~ 0x0009efff
  memman_free(memman, 0x00400000, memtotal - 0x00400000);

  init_palette();
  shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
  sht_back = sheet_alloc(shtctl);
  sht_mouse = sheet_alloc(shtctl);
  sht_win = sheet_alloc(shtctl);
  buf_back =
      (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
  buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 52);
  sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny,
               -1);                               // 没有透明色
  sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99); // 透明色号99
  sheet_setbuf(sht_win, buf_win, 160, 52, -1);    // 没有透明色
  init_screen8(buf_back, binfo->scrnx, binfo->scrny);
  init_mouse_cursor8(buf_mouse, 99); // 背景色号99
  make_window8(buf_win, 160, 52, "counter");
  make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
  int cursor_x = 8;
  int cursor_c = COL8_FFFFFF;
  sheet_slide(sht_back, 0, 0);
  int mx = (binfo->scrnx - 16) / 2; // 按在画面中央来计算坐标
  int my = (binfo->scrny - 28 - 16) / 2;
  sheet_slide(sht_mouse, mx, my);
  sheet_slide(sht_win, 80, 72);
  sheet_updown(sht_back, 0);
  sheet_updown(sht_win, 1);
  sheet_updown(sht_mouse, 2);
  sprintf(s, "(%3d, %3d)", mx, my);
  put_fonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
  sprintf(s, "memory %dMB, free: %dKB", memtotal / (1024 * 1024),
          memman_total(memman) / 1024);
  put_fonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
  sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48);

  for (;;) {
    io_cli();
    if (fifo32_status(&fifo) == 0) {
      io_stihlt();
    } else {
      data = fifo32_get(&fifo);
      io_sti();

      if (256 <= data && data <= 511) {
        // 键盘数据
        sprintf(s, "%02X", data - 256);
        put_fonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
        if (data < 0x54 + 256) {
          if (keytable[data - 256] && cursor_x < 144) {
            // 一般字符
            s[0] = keytable[data - 256];
            s[1] = '\0';
            put_fonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
            cursor_x += 8;
          }
          if (data == 256 + 0x0e && cursor_x > 8) {
            // 退格键
            // 用空格键把光标消去后，后移一次光标
            put_fonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
            cursor_x -= 8;
          }
          box_fill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
          sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
        }
      } else if (512 <= data && data <= 767) {
        // 鼠标数据
        if (mouse_decode(&mdec, data - 512)) {
          sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
          if ((mdec.btn & 0x01)) {
            s[1] = 'L';
          }
          if ((mdec.btn & 0x02)) {
            s[3] = 'R';
          }
          if ((mdec.btn & 0x04)) {
            s[2] = 'C';
          }

          put_fonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);

          // 移动光标
          mx += mdec.x;
          my += mdec.y;

          if (mx < 0) {
            mx = 0;
          }
          if (my < 0) {
            my = 0;
          }
          if (mx > binfo->scrnx - 1) {
            mx = binfo->scrnx - 1;
          }
          if (my > binfo->scrny - 1) {
            my = binfo->scrny - 1;
          }

          sprintf(s, "(%3d, %3d)", mx, my);
          put_fonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
          sheet_slide(sht_mouse, mx, my);

          if (mdec.btn & 0x01) {
            // 按下左键，移动sht_win
            sheet_slide(sht_win, mx - 80, my - 8);
          }
        }
      } else if (data == 10) {
        put_fonts8_asc_sht(sht_back, 0, 64, COL8_FFFFFF, COL8_008484, "10[sec]",
                           7);
      } else if (data == 3) {
        put_fonts8_asc_sht(sht_back, 0, 80, COL8_FFFFFF, COL8_008484, "3[sec]",
                           6);
      } else if (data <= 1) {
        if (data) {
          timer_init(timer3, &fifo, 0);
          cursor_c = COL8_000000;
        } else {
          timer_init(timer3, &fifo, 1);
          cursor_c = COL8_FFFFFF;
        }

        timer_set_timer(timer3, 50);
        box_fill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
        sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
      }
    }
  }

  return 0;
}
