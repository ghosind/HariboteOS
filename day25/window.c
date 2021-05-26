#include "graphic.h"
#include "sheet.h"
#include "window.h"

void make_window8(unsigned char *buf, int xsize, int ysize, char *title,
                  char act) {
  box_fill8(buf, xsize, COL8_C6C6C6, 0, 0, xsize - 1, 0);
  box_fill8(buf, xsize, COL8_FFFFFF, 1, 1, xsize - 2, 1);
  box_fill8(buf, xsize, COL8_C6C6C6, 0, 0, 0, ysize - 1);
  box_fill8(buf, xsize, COL8_FFFFFF, 1, 1, 1, ysize - 2);
  box_fill8(buf, xsize, COL8_848484, xsize - 2, 1, xsize - 2, ysize - 2);
  box_fill8(buf, xsize, COL8_000000, xsize - 1, 0, xsize - 1, ysize - 1);
  box_fill8(buf, xsize, COL8_C6C6C6, 2, 2, xsize - 3, ysize - 3);
  box_fill8(buf, xsize, COL8_848484, 1, ysize - 2, xsize - 2, ysize - 2);
  box_fill8(buf, xsize, COL8_000000, 0, ysize - 1, xsize - 1, ysize - 1);
  make_window_title8(buf, xsize, title, act);
}

void make_textbox8(struct Sheet *sht, int x0, int y0, int sx, int sy, int c) {
  int x1 = x0 + sx, y1 = y0 + sy;

  box_fill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
  box_fill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
  box_fill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
  box_fill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
  box_fill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
  box_fill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
  box_fill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 1, y1 + 1);
  box_fill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 0, y1 + 1);
  box_fill8(sht->buf, sht->bxsize, c, x0 - 1, y0 - 1, x1 + 0, y1 + 0);
}

void make_window_title8(unsigned char *buf, int xsize, char *title, char act) {
  static char closebtn[14][16] = {
      "OOOOOOOOOOOOOOO@",
      "OQQQQQQQQQQQQQ$@",
      "OQQQQQQQQQQQQQ$@",
      "OQQQ@@QQQQ@@QQ$@",
      "OQQQQ@@QQ@@QQQ$@",
      "OQQQQQ@@@@QQQQ$@",
      "OQQQQQQ@@QQQQQ$@",
      "OQQQQQ@@@@QQQQ$@",
      "OQQQQ@@QQ@@QQQ$@",
      "OQQQ@@QQQQ@@QQ$@",
      "OQQQQQQQQQQQQQ$@",
      "OQQQQQQQQQQQQQ$@",
      "O$$$$$$$$$$$$$$@",
      "@@@@@@@@@@@@@@@@"};
  char tc, tbc;

  if (act) {
    tc = COL8_FFFFFF;
    tbc = COL8_000084;
  } else {
    tc = COL8_C6C6C6;
    tbc = COL8_848484;
  }

  box_fill8(buf, xsize, tbc, 3, 3, xsize - 4, 20);
  put_fonts8_asc(buf, xsize, 24, 4, tc, title);

  for (int y = 0; y < 14; y++) {
    for (int x = 0; x < 16; x++) {
      char c = closebtn[y][x];
      switch (c) {
      case '@':
        c = COL8_000000;
        break;
      case '$':
        c = COL8_848484;
        break;
      case 'Q':
        c = COL8_C6C6C6;
        break;
      default:
        c = COL8_FFFFFF;
        break;
      }
      buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
    }
  }
}

void change_window_title8(struct Sheet *sht, char act) {
  int xsize = sht->bxsize;
  char tc_new, tbc_new, tc_old, tbc_old;
  unsigned char *buf = sht->buf;

  if (act) {
    tc_new = COL8_FFFFFF;
    tbc_new = COL8_000084;
    tc_old = COL8_C6C6C6;
    tbc_old = COL8_848484;
  } else {
    tc_new = COL8_C6C6C6;
    tbc_new = COL8_848484;
    tc_old = COL8_FFFFFF;
    tbc_old = COL8_000084;
  }

  for (int y = 3; y <= 20; y++) {
    for (int x = 3; x <= xsize - 4; x++) {
      char c = buf[y * xsize + x];
      if (c == tc_old && x <= xsize - 22) {
        c = tc_new;
      } else if (c == tbc_old) {
        c = tbc_new;
      }
      buf[y * xsize + x] = c;
    }
  }

  sheet_refresh(sht, 3, 3, xsize, 21);
}

void keywin_off(struct Sheet *key_win) {
  change_window_title8(key_win, 0);

  if (key_win->flags & 0x20) {
    fifo32_put(&key_win->task->fifo, 2);
  }
}

void keywin_on(struct Sheet *key_win) {
  change_window_title8(key_win, 1);

  if (key_win->flags & 0x20) {
    fifo32_put(&key_win->task->fifo, 2);
  }
}
