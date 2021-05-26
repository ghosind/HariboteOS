#include <stdio.h>

#include "api.h"
#include "console.h"
#include "graphic.h"
#include "io.h"
#include "sheet.h"
#include "task.h"
#include "window.h"

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
             int eax) {
  struct Task *task = task_now();
  int ds_base = task->ds_base;
  struct Console *cons = task->cons;
  struct Shtctl *shtctl = (struct Shtctl *)*((int *)0x0fe4);
  struct Sheet *sht;
  char s[12];
  int *reg = &eax + 1;
  int data;

  switch (edx) {
  case 1:
    cons_putchar(cons, eax & 0xff, 1);
    break;
  case 2:
    cons_putstr(cons, (char *)ebx + ds_base);
    break;
  case 3:
    cons_putnstr(cons, (char *)ebx + ds_base, ecx);
    break;
  case 4:
    return &(task->tss.esp0);
    break;
  case 5:
    sht = sheet_alloc(shtctl);
    sht->task = task;
    sht->flags |= 0x10;
    sheet_setbuf(sht, (unsigned char *)(ebx + ds_base), esi, edi, eax);
    make_window8((unsigned char *)(ebx + ds_base), esi, edi,
                 (char *)(ecx + ds_base), 0);
    sheet_slide(sht, (shtctl->xsize - esi) / 2, (shtctl->ysize - edi) / 2);
    sheet_updown(sht, shtctl->top);
    reg[7] = (int)sht;
    break;
  case 6:
    sht = (struct Sheet *)(ebx & 0xfffffffe);
    put_fonts8_asc(sht->buf, sht->bxsize, esi, edi, eax,
                   (char *)(ebp + ds_base));
    if (!(ebx & 1)) {
      sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
    }
    break;
  case 7:
    sht = (struct Sheet *)(ebx & 0xfffffffe);
    box_fill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
    if (!(ebx & 1)) {
      sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
    }
    break;
  case 8:
    memman_init((struct MemMan *)(ebx + ds_base));
    ecx &= 0xfffffff0;
    memman_free((struct MemMan *)(ebx + ds_base), eax, ecx);
    break;
  case 9:
    ecx = (ecx + 0x0f) & 0xfffffff0;
    reg[7] = memman_alloc((struct MemMan *)(ebx + ds_base), ecx);
    break;
  case 10:
    ecx = (ecx + 0x0f) & 0xfffffff0;
    memman_free((struct MemMan *)(ebx + ds_base), eax, ecx);
    break;
  case 11:
    sht = (struct Sheet *)(ebx & 0xfffffffe);
    sht->buf[sht->bxsize * edi + esi] = eax;
    if (!(ebx & 1)) {
      sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
    }
    break;
  case 12:
    sht = (struct Sheet *)ebx;
    sheet_refresh(sht, eax, ecx, esi, edi);
    break;
  case 13:
    sht = (struct Sheet *)(ebx & 0xfffffffe);
    api_line_win(sht, eax, ecx, esi, edi, ebp);
    if (!(ebx & 1)) {
      sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
    }
    break;
  case 14:
    sheet_free((struct Sheet *)ebx);
    break;
  case 15:
    for (;;) {
      io_cli();

      if (!fifo32_status(&task->fifo)) {
        if (eax) {
          task_sleep(task);
        } else {
          io_sti();
          reg[7] = -1;
          return 0;
        }
      }

      data = fifo32_get(&task->fifo);
      io_sti();
      if (data <= 1) {
        timer_init(cons->timer, &task->fifo, 1);
        timer_set_timer(cons->timer, 50);
      }
      if (data == 2) {
        cons->cur_c = COL8_FFFFFF;
      }
      if (data == 3) {
        cons->cur_c = -1;
      }
      if (256 <= data) {
        reg[7] = data - 256;
        return 0;
      }
    }

    break;
  case 16:
    reg[7] = (int)timer_alloc();
    ((struct Timer *)reg[7])->flags2 = 1;
    break;
  case 17:
    timer_init((struct Timer *)ebx, &task->fifo, eax + 256);
    break;
  case 18:
    timer_set_timer((struct Timer *)ebx, eax);
    break;
  case 19:
    timer_free((struct Timer *)ebx);
    break;
  case 20:
    if (!eax) {
      data = io_in8(0x61);
      io_out8(0x61, data & 0x0d);
    } else {
      data = 1193180000 / eax;

      io_out8(0x43, 0xb6);
      io_out8(0x42, data & 0xff);
      io_out8(0x42, data >> 8);
      
      data = io_in8(0x61);
      io_out8(0x61, (data | 0x03) & 0x0f);
    }
    break;
  default:
    break;
  }

  return 0;
}

void api_line_win(struct Sheet *sht, int x0, int y0, int x1, int y1, int col) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int x = x0 << 10;
  int y = y0 << 10;
  int len = 0;

  if (dx < 0) {
    dx = -dx;
  }
  if (dy < 0) {
    dy = -dy;
  }

  if (dx >= dy) {
    len = dx + 1;
    if (x0 > x1) {
      dx = -1024;
    } else {
      dx = 1024;
    }

    if (y0 <= y1) {
      dy = ((y1 - y0 + 1) << 10) / len;
    } else {
      dy = ((y1 - y0 - 1) << 10) / len;
    }
  } else {
    len = dy + 1;
    if (y0 > y1) {
      dy = -1024;
    } else {
      dy = 1024;
    }

    if (x0 <= x1) {
      dx = ((x1 - x0 + 1) << 10) / len;
    } else {
      dx = ((x1 - x0 - 1) << 10) / len;
    }
  }

  for (int i = 0; i < len; i++) {
    sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
    x += dx;
    y += dy;
  }
}
