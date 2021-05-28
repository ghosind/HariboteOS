#include <stdio.h>
#include <string.h>

#include "bootpack.h"
#include "command.h"
#include "console.h"
#include "desctbl.h"
#include "fifo.h"
#include "fs.h"
#include "graphic.h"
#include "io.h"
#include "memory.h"
#include "mouse.h"
#include "sheet.h"
#include "task.h"
#include "timer.h"
#include "window.h"

void console_task(struct Sheet *sheet, unsigned int memtotal) {
  struct Task *task = task_now();
  char s[30], cmdline[30];
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  int *fat = (int *)memman_alloc_4k(memman, 4 * 2880);
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
  int x, y;
  struct Console cons;

  cons.sheet = sheet;
  cons.cur_x = 8;
  cons.cur_y = 28;
  cons.cur_c = -1;
  task->cons = &cons;

  if (sheet) {
    cons.timer = timer_alloc();
    timer_init(cons.timer, &task->fifo, 1);
    timer_set_timer(cons.timer, 50);
  }

  file_read_fat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));

  cons_putchar(&cons, '>', 1);

  for (;;) {
    io_cli();

    if (!fifo32_status(&task->fifo)) {
      task_sleep(task);
      io_sti();
    } else {
      int data = fifo32_get(&task->fifo);
      io_sti();
      if (data <= 1) {
        if (data) {
          timer_init(cons.timer, &task->fifo, 0);
          if (cons.cur_c >= 0) {
            cons.cur_c = COL8_FFFFFF;
          }
        } else {
          timer_init(cons.timer, &task->fifo, 1);
          if (cons.cur_c >= 0) {
            cons.cur_c = COL8_000000;
          }
        }

        timer_set_timer(cons.timer, 50);
      }

      // 光标ON
      if (data == 2) {
        cons.cur_c = COL8_FFFFFF;
      }
      // 光标OFF
      if (data == 3) {
        box_fill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x,
                  cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
        cons.cur_c = -1;
      }

      if (data == 4) {
        cmd_exit(&cons, fat);
      }

      if (data >= 256 && data <= 511) {
        if (data == 8 + 256) {
          // 退格键
          if (cons.cur_x > 16) {
            // 用空白擦除光标后将光标前移一位
            cons_putchar(&cons, ' ', 0);
            cons.cur_x -= 8;
          }
        } else if (data == 10 + 256) {
          // 回车键
          // 用空格将光标擦除
          cons_putchar(&cons, ' ', 0);
          cmdline[cons.cur_x / 8 - 2] = '\0';
          cons_newline(&cons);
          cons_run_cmd(cmdline, &cons, fat, memtotal);

          if (!sheet) {
            cmd_exit(&cons, fat);
          }

          cons_putchar(&cons, '>', 1);
        } else {
          if (cons.cur_x < 240) {
            cmdline[cons.cur_x / 8 - 2] = data - 256;
            cons_putchar(&cons, data - 256, 1);
          }
        }
      }

      if (sheet) {
        if (cons.cur_c >= 0) {
          box_fill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x,
                    cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
        }
        sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8,
                      cons.cur_y + 16);
      }
    }
  }
}

void cons_putchar(struct Console *cons, int ch, char move) {
  char s[2];

  s[0] = ch;
  s[1] = '\0';

  if (s[0] == '\t') {
    for (;;) {
      if (cons->sheet) {
        put_fonts8_asc_sht(cons->sheet, cons->cur_x, cons->cur_y, COL8_FFFFFF,
                           COL8_000000, " ", 1);
      }

      cons->cur_x += 8;
      if (cons->cur_x == 8 + 240) {
        cons_newline(cons);
      }

      if (!((cons->cur_x - 8) & 0x1f)) {
        break; // 被32整除则break
      }
    }
  } else if (s[0] == '\n') {
    // 换行符
    cons_newline(cons);
  } else if (s[0] == '\r') {
    // 回车符，暂不处理
  } else {
    if (cons->sheet) {
      put_fonts8_asc_sht(cons->sheet, cons->cur_x, cons->cur_y, COL8_FFFFFF,
                         COL8_000000, s, 1);
    }

    if (move) {
      cons->cur_x += 8;
      if (cons->cur_x == 8 + 240) {
        cons_newline(cons);
      }
    }
  }
}

void cons_newline(struct Console *cons) {
  struct Sheet *sheet = cons->sheet;

  if (cons->cur_y < 28 + 112) {
    cons->cur_y += 16;
  } else {
    // 滚动
    if (sheet) {
      for (int y = 28; y < 28 + 112; y++) {
        for (int x = 8; x < 8 + 240; x++) {
          sheet->buf[x + y * sheet->bxsize] =
              sheet->buf[x + (y + 16) * sheet->bxsize];
        }
      }
      for (int y = 28 + 112; y < 28 + 128; y++) {
        for (int x = 8; x < 8 + 240; x++) {
          sheet->buf[x + y * sheet->bxsize] = COL8_000000;
        }
      }

      sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
    }
  }

  cons->cur_x = 8;
}

void cons_putstr(struct Console *cons, char *s) {
  while (*s) {
    cons_putchar(cons, *s, 1);
    s++;
  }
}

void cons_putnstr(struct Console *cons, char *s, int n) {
  for (int i = 0; i < n; i++) {
    cons_putchar(cons, s[i], 1);
  }
}

void cons_run_cmd(char *cmdline, struct Console *cons, int *fat,
                  unsigned int memtotal) {
  if (!strcmp(cmdline, "mem") && cons->sheet) {
    cmd_mem(cons, memtotal);
  } else if (!strcmp(cmdline, "cls") && cons->sheet) {
    cmd_cls(cons);
  } else if (!strcmp(cmdline, "dir") && cons->sheet) {
    cmd_dir(cons);
  } else if (!strncmp(cmdline, "type ", 5) && cons->sheet) {
    cmd_type(cons, fat, cmdline);
  } else if (!strcmp(cmdline, "exit")) {
    cmd_exit(cons, fat);
  } else if (!strncmp(cmdline, "start ", 6)) {
    cmd_start(cons, cmdline, memtotal);
  } else if (!strncmp(cmdline, "ncst ", 5)) {
    cmd_ncst(cons, cmdline, memtotal);
  } else if (strcmp(cmdline, "")) {
    if (!cmd_app(cons, fat, cmdline)) {
      cons_putstr(cons, "Bad command.\n\n");
    }
  }
}

struct Task *open_cons_task(struct Sheet *sht, unsigned int memtotal) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct Task *task = task_alloc();
  int *cons_fifo = (int *)memman_alloc_4k(memman, 128 * 4);

  task->cons_stack = memman_alloc(memman, 64 * 1024);
  task->tss.esp = task->cons_stack + 64 * 1024 - 12;
  task->tss.eip = (int)&console_task;
  task->tss.es = 1 * 8;
  task->tss.cs = 2 * 8;
  task->tss.ss = 1 * 8;
  task->tss.ds = 1 * 8;
  task->tss.fs = 1 * 8;
  task->tss.gs = 1 * 8;
  *((int *)(task->tss.esp + 4)) = (int)sht;
  *((int *)(task->tss.esp + 8)) = memtotal;
  task_run(task, 2, 2);
  fifo32_init(&task->fifo, 128, cons_fifo, task);

  return task;
}

struct Sheet *open_console(struct Shtctl *shtctl, unsigned int memtotal) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct Sheet *sht = sheet_alloc(shtctl);
  unsigned char *buf = (unsigned char *)memman_alloc_4k(memman, 256 * 165);

  sheet_setbuf(sht, buf, 256, 165, -1);
  make_window8(buf, 256, 165, "console", 0);
  make_textbox8(sht, 8, 28, 240, 128, COL8_000000);

  sht->task = open_cons_task(sht, memtotal);
  sht->flags |= 0x20;

  return sht;
}

void close_cons_task(struct Task *task) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  task_sleep(task);
  memman_free_4k(memman, task->cons_stack, 64 * 1024);
  memman_free_4k(memman, (int)task->fifo.buf, 128 * 4);
  task->flags = 0;
}

void close_console(struct Sheet *sht) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct Task *task = (struct Task *)sht->task;

  memman_free_4k(memman, (int)sht->buf, 256 * 165);
  sheet_free(sht);
  close_cons_task(task);
}
