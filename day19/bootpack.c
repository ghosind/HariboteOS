#include <stdio.h>

#include "bootpack.h"
#include "console.h"
#include "desctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "sheet.h"
#include "task.h"
#include "timer.h"
#include "window.h"

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *)ADR_BOOTINFO;
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct MouseDec mdec;
  char s[40], mcursor[256];
  unsigned int memtotal;
  struct Shtctl *shtctl;
  struct Sheet *sht_back, *sht_mouse, *sht_win, *sht_cons;
  unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_cons;
  struct Timer *timer;
  struct FIFO32 fifo, keycmd;
  int fifobuf[128], keycmd_buf[32];
  int data, key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7,
            keycmd_wait = -1;

  init_gdtidt();
  init_pic(); // GDT/IDT完成初始化，开放CPU中断

  io_sti();

  fifo32_init(&fifo, 128, fifobuf, NULL);
  fifo32_init(&keycmd, 32, keycmd_buf, NULL);

  init_keyboard(&fifo, 256);
  enable_mouse(&fifo, 512, &mdec);

  init_pit();

  io_out8(PIC0_IMR, 0xf8); // 开放PIT、PIC1以及键盘中断
  io_out8(PIC1_IMR, 0xef); // 开放鼠标中断

  memtotal = memtest(0x00400000, 0xbfffffff);
  memman_init(memman);
  // 书中为0x00001000 ~ 0x0009e000
  // 注1: 测试时发现会造成错误（原因未知），所以改为由0x00010000开始
  // 注2: 在640*480模式下，free该段内存后会导致屏幕画面错误
  // memman_free(memman, 0x00010000, 0x0009e000); // 0x00010000 ~ 0x0009efff
  memman_free(memman, 0x00400000, memtotal - 0x00400000);

  init_palette();
  shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
  struct Task *task_a = task_init(memman);
  fifo.task = task_a;
  task_run(task_a, 1, 0);

  // sht_back
  sht_back = sheet_alloc(shtctl);
  buf_back =
      (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
  sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny,
               -1); // 没有透明色
  init_screen8(buf_back, binfo->scrnx, binfo->scrny);

  // sht_cons
  sht_cons = sheet_alloc(shtctl);
  buf_cons = (unsigned char *)memman_alloc_4k(memman, 256 * 165);
  sheet_setbuf(sht_cons, buf_cons, 256, 165, -1); // 无透明色
  make_window8(buf_cons, 256, 165, "console", 0);
  make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);
  struct Task *task_cons = task_alloc();
  task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
  task_cons->tss.eip = (int)&console_task;
  task_cons->tss.es = 1 * 8;
  task_cons->tss.cs = 2 * 8;
  task_cons->tss.ss = 1 * 8;
  task_cons->tss.ds = 1 * 8;
  task_cons->tss.fs = 1 * 8;
  task_cons->tss.gs = 1 * 8;
  *((int *)(task_cons->tss.esp + 4)) = (int)sht_cons;
  *((int *)(task_cons->tss.esp + 8)) = memtotal;
  task_run(task_cons, 2, 2);

  // sht_win
  sht_win = sheet_alloc(shtctl);
  buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 52);
  sheet_setbuf(sht_win, buf_win, 160, 52, -1); // 没有透明色
  make_window8(buf_win, 160, 52, "task_a", 1);
  make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
  int cursor_x = 8;
  int cursor_c = COL8_FFFFFF;
  timer = timer_alloc();
  timer_init(timer, &fifo, 1);
  timer_set_timer(timer, 50);

  // sht_mouse
  sht_mouse = sheet_alloc(shtctl);
  sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99); // 透明色号99
  init_mouse_cursor8(buf_mouse, 99);              // 背景色号99
  int mx = (binfo->scrnx - 16) / 2; // 按在画面中央来计算坐标
  int my = (binfo->scrny - 28 - 16) / 2;

  sheet_slide(sht_back, 0, 0);
  sheet_slide(sht_cons, 32, 4);
  sheet_slide(sht_win, 64, 56);
  sheet_slide(sht_mouse, mx, my);
  sheet_updown(sht_back, 0);
  sheet_updown(sht_cons, 1);
  sheet_updown(sht_win, 2);
  sheet_updown(sht_mouse, 3);

  // 避免与键盘状态冲突
  fifo32_put(&keycmd, KEYCMD_LED);
  fifo32_put(&keycmd, key_leds);

  for (;;) {
    if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
      // 如果存在向键盘控制器发送的数据
      keycmd_wait = fifo32_get(&keycmd);
      wait_KBC_sendready();
      io_out8(PORT_KEYCMD, keycmd_wait);
    }

    io_cli();
    if (fifo32_status(&fifo) == 0) {
      task_sleep(task_a);
      io_sti();
    } else {
      data = fifo32_get(&fifo);
      io_sti();

      if (256 <= data && data <= 511) {
        // 键盘数据
        if (data < 0x80 + 256) {
          if (key_shift == 0) {
            s[0] = keytable0[data - 256];
          } else {
            s[0] = keytable1[data - 256];
          }
        } else {
          s[0] = 0;
        }

        if ('A' <= s[0] && s[0] <= 'Z') {
          if ((!(key_leds & 4) && !key_shift) ||
              ((key_leds & 4) && key_shift)) {
            s[0] += 0x20;
          }
        }

        if (s[0]) {
          if (!key_to) {
            if (cursor_x < 128) {
              // 显示一个字符之后将光标后移一位
              s[1] = '\0';
              put_fonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000,
                                 COL8_FFFFFF, s, 1);
              cursor_x += 8;
            }
          } else {
            fifo32_put(&task_cons->fifo, s[0] + 256);
          }
        }

        if (data == 256 + 0x0e) {
          // 退格键
          if (!key_to) {
            if (cursor_x > 8) {
              // 用空格键把光标消去后，后移一次光标
              put_fonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000,
                                 COL8_FFFFFF, " ", 1);
              cursor_x -= 8;
            }
          } else {
            fifo32_put(&task_cons->fifo, 8 + 256);
          }
        }

        // Tab键
        if (data == 256 + 0x0f) {
          if (!key_to) {
            key_to = 1;
            make_window_title8(buf_win, sht_win->bxsize, "task_a", 0);
            make_window_title8(buf_cons, sht_cons->bxsize, "console", 1);
            cursor_c = -1; // 不显示光标
            box_fill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28,
                      cursor_x + 7, 43);
            fifo32_put(&task_cons->fifo, 2); // 命令行窗口光标ON
          } else {
            key_to = 0;
            make_window_title8(buf_win, sht_win->bxsize, "task_a", 1);
            make_window_title8(buf_cons, sht_cons->bxsize, "console", 0);
            cursor_c = COL8_000000;          // 显示光标
            fifo32_put(&task_cons->fifo, 3); // 命令行窗口光标OFF
          }

          sheet_refresh(sht_win, 0, 0, sht_win->bxsize, 21);
          sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
        }

        // 回车键
        if (data == 256 + 0x1c) {
          if (key_to) {
            fifo32_put(&task_cons->fifo, 10 + 256);
          }
        }

        // 左Shift按下
        if (data == 256 + 0x2a) {
          key_shift |= 1;
        }
        // 右Shift按下
        if (data == 256 + 0x36) {
          key_shift |= 2;
        }
        // 左Shift释放
        if (data == 256 + 0xaa) {
          key_shift &= ~1;
        }
        // 右Shift释放
        if (data == 256 + 0xb6) {
          key_shift &= ~2;
        }

        // CapsLock
        if (data == 256 + 0x3a) {
          key_leds ^= 4;
          fifo32_put(&keycmd, KEYCMD_LED);
          fifo32_put(&keycmd, key_leds);
        }
        // NumLock
        if (data == 256 + 0x45) {
          key_leds ^= 2;
          fifo32_put(&keycmd, KEYCMD_LED);
          fifo32_put(&keycmd, key_leds);
        }
        // ScrollLock
        if (data == 256 + 0x46) {
          key_leds ^= 1;
          fifo32_put(&keycmd, KEYCMD_LED);
          fifo32_put(&keycmd, key_leds);
        }

        // 键盘成功接收到数据
        if (data == 256 + 0xfa) {
          keycmd_wait = -1;
        }
        if (data == 256 + 0xfe) {
          wait_KBC_sendready();
          io_out8(PORT_KEYDAT, keycmd_wait);
        }

        if (cursor_c >= 0) {
          box_fill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28,
                    cursor_x + 7, 43);
        }

        sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
      } else if (512 <= data && data <= 767) {
        // 鼠标数据
        if (mouse_decode(&mdec, data - 512)) {
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

          sheet_slide(sht_mouse, mx, my);

          if (mdec.btn & 0x01) {
            // 按下左键，移动sht_win
            sheet_slide(sht_win, mx - 80, my - 8);
          }
        }
      } else if (data <= 1) {
        if (data) {
          timer_init(timer, &fifo, 0);
          if (cursor_c >= 0) {
            cursor_c = COL8_000000;
          }
        } else {
          timer_init(timer, &fifo, 1);
          if (cursor_c >= 0) {
            cursor_c = COL8_FFFFFF;
          }
        }

        timer_set_timer(timer, 50);
        if (cursor_c >= 0) {
          box_fill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28,
                    cursor_x + 7, 43);
          sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
        }
      }
    }
  }

  return 0;
}
