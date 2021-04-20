#include <stdio.h>
#include <string.h>

#include "bootpack.h"
#include "console.h"
#include "desctbl.h"
#include "fifo.h"
#include "fs.h"
#include "graphic.h"
#include "memory.h"
#include "mouse.h"
#include "io.h"
#include "sheet.h"
#include "task.h"
#include "timer.h"

void console_task(struct Sheet *sheet, unsigned int memtotal) {
  struct Task *task = task_now();
  int fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
  char s[30], cmdline[30];
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
  int *fat = (int *)memman_alloc_4k(memman, 4 * 2880);
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
  int x, y;

  fifo32_init(&task->fifo, 128, fifobuf, task);

  struct Timer *timer = timer_alloc();
  timer_init(timer, &task->fifo, 1);
  timer_set_timer(timer, 50);

  file_read_fat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));

  put_fonts8_asc_sht(sheet, 8, 28, COL8_FFFFFF, COL8_000000, ">", 1);

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
          timer_init(timer, &task->fifo, 0);
          if (cursor_c >= 0) {
            cursor_c = COL8_FFFFFF;
          }
        } else {
          timer_init(timer, &task->fifo, 1);
          if (cursor_c >= 0) {
            cursor_c = COL8_000000;
          }
        }

        timer_set_timer(timer, 50);
      }

      // 光标ON
      if (data == 2) {
        cursor_c = COL8_FFFFFF;
      }
      // 光标OFF
      if (data == 3) {
        box_fill8(sheet->buf, sheet->bxsize, COL8_000000, cursor_x, 28,
                  cursor_x + 7, 43);
        cursor_c = -1;
      }

      if (data >= 256 && data <= 511) {
        if (data == 8 + 256) {
          // 退格键
          if (cursor_x > 16) {
            // 用空白擦除光标后将光标前移一位
            put_fonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF,
                               COL8_000000, " ", 1);
            cursor_x -= 8;
          }
        } else if (data == 10 + 256) {
          // 回车键
          // 用空格将光标擦除
          put_fonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF,
                             COL8_000000, " ", 1);

          cmdline[cursor_x / 8 - 2] = '\0';
          cursor_y = cons_newline(cursor_y, sheet);

          if (!strcmp(cmdline, "mem")) {
            // mem命令
            sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
            put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s,
                               30);
            cursor_y = cons_newline(cursor_y, sheet);
            sprintf(s, "free %dKB", memman_total(memman) / 1024);
            put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s,
                               30);
            cursor_y = cons_newline(cursor_y, sheet);
            cursor_y = cons_newline(cursor_y, sheet);
          } else if (!strcmp(cmdline, "cls")) {
            // cls命令
            for (int y = 28; y < 28 + 128; y++) {
              for (int x = 8; x < 8 + 240; x++) {
                sheet->buf[x + y * sheet->bxsize] = COL8_000000;
              }
            }

            sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
            cursor_y = 28;
          } else if (!strcmp(cmdline, "dir")) {
            // dir命令
            for (x = 0; x < 224; x++) {
              if (finfo[x].name[0] == '\0') {
                break;
              }

              if (finfo[x].name[0] != 0xe5) {
                if (!(finfo[x].type & 0x18)) {
                  sprintf(s, "filename.ext   %7d", finfo[x].size);
                  for (y = 0; y < 8; y++) {
                    s[y] = finfo[x].name[y];
                  }
                  s[9] = finfo[x].ext[0];
                  s[10] = finfo[x].ext[1];
                  s[11] = finfo[x].ext[2];
                  put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF,
                                     COL8_000000, s, 30);
                  cursor_y = cons_newline(cursor_y, sheet);
                }
              }
            }
            cursor_y = cons_newline(cursor_y, sheet);
          } else if (!strncmp(cmdline, "type ", 5)) {
            // type命令
            for (y = 0; y < 11; y++) {
              s[y] = ' ';
            }
            y = 0;

            for (x = 5; y < 11 && cmdline[x] != '\0'; x++) {
              if (cmdline[x] == '.' && s[y] <= 'z') {
                y = 8;
              } else {
                s[y] = cmdline[x];
                if ('a' <= s[y] && s[y] <= 'z') {
                  // 转为大写
                  s[y] -= 0x20;
                }
                y++;
              }
            }

            // 寻找文件
            for (x = 0; x < 224;) {
              if (finfo[x].name[0] == '\0') {
                break;
              }
              if (!(finfo[x].type & 0x18)) {
                for (y = 0; y < 11; y++) {
                  if (finfo[x].name[y] != s[y]) {
                    goto type_next_file;
                  }
                }
                break;
              }

            type_next_file:
              x++;
            }

            if (x < 224 && finfo[x].name[0] != '\0') {
              char *p = (char *)memman_alloc_4k(memman, finfo[x].size);
              file_load_file(finfo[x].clustno, finfo[x].size, p, fat,
                             (char *)(ADR_DISKIMG + 0x003e00));
              cursor_x = 8;
              for (y = 0; y < finfo[x].size; y++) {
                s[0] = p[y];
                s[1] = '\0';
                if (s[0] == '\t') {
                  for (;;) {
                    put_fonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF,
                                       COL8_000000, " ", 1);
                    cursor_x += 8;
                    if (cursor_x == 8 + 240) {
                      cursor_x = 8;
                      cursor_y = cons_newline(cursor_y, sheet);
                    }
                    if (!((cursor_x - 8) & 0x1f)) {
                      break; // 被32整除则break
                    }
                  }
                } else if (s[0] == '\n') {
                  // 换行符
                  cursor_x = 8;
                  cursor_y = cons_newline(cursor_y, sheet);
                } else if (s[0] == '\r') {
                  // 回车符，暂不处理
                } else {
                  put_fonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF,
                                     COL8_000000, s, 1);
                  cursor_x += 8;
                  if (cursor_x == 8 + 240) {
                    cursor_x = 8;
                    cursor_y = cons_newline(cursor_y, sheet);
                  }
                }
              }

              memman_free_4k(memman, (int)p, finfo[x].size);
            } else {
              put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000,
                                 "File not found.", 15);
              cursor_y = cons_newline(cursor_y, sheet);
            }

            cursor_y = cons_newline(cursor_y, sheet);
          } else if (!strcmp(cmdline, "hlt")) {
            for (y = 0; y < 11; y++) {
              s[y] = ' ';
            }

            s[0] = 'H';
            s[1] = 'L';
            s[2] = 'T';
            s[8] = 'H';
            s[9] = 'R';
            s[10] = 'B';
            
            for (x = 0; x < 224; ) {
              if (finfo[x].name[0] == '\0') {
                break;
              }
              if (!(finfo[x].type & 0x18)) {
                for (y = 0; y < 11; y++) {
                  if (finfo[x].name[y] != s[y]) {
                    goto hlt_next_file;
                  }
                }
                break;
              }
              hlt_next_file:
              x++;
            }

            if (x < 224 && finfo[x].name[0] != '\0') {
              char *p = (char *) memman_alloc_4k(memman, finfo[x].size);
              file_load_file(finfo[x].clustno, finfo[x].size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
              set_segmdesc(gdt + 1003, finfo[x].size - 1, (int) p, AR_CODE32_ER);
              far_jmp(0, 1003 * 8);
              memman_free_4k(memman, (int) p, finfo[x].size);
            } else {
              put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
              cursor_y = cons_newline(cursor_y, sheet);
            }
            cursor_y = cons_newline(cursor_y, sheet);
          } else if (strcmp(cmdline, "")) {
            put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000,
                               "Bad command.", 12);
            cursor_y = cons_newline(cursor_y, sheet);
            cursor_y = cons_newline(cursor_y, sheet);
          }

          // 显示提示符
          put_fonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, ">",
                             1);
          cursor_x = 16;
        } else {
          if (cursor_x < 240) {
            s[0] = data - 256;
            s[1] = '\0';
            cmdline[cursor_x / 8 - 2] = data - 256;
            put_fonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF,
                               COL8_000000, s, 1);
            cursor_x += 8;
          }
        }
      }

      if (cursor_c >= 0) {
        box_fill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y,
                  cursor_x + 7, cursor_y + 15);
      }
      sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
    }
  }
}

int cons_newline(int cursor_y, struct Sheet *sheet) {
  if (cursor_y < 28 + 112) {
    cursor_y += 16;
  } else {
    // 滚动
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

  return cursor_y;
}
