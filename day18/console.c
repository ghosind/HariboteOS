#include "console.h"
#include "graphic.h"
#include "sheet.h"

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
