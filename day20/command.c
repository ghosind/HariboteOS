#include <stdio.h>

#include "bootpack.h"
#include "command.h"
#include "console.h"
#include "desctbl.h"
#include "fs.h"
#include "graphic.h"
#include "memory.h"
#include "sheet.h"
#include "task.h"

void cmd_mem(struct Console *cons, unsigned int memtotal) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  char s[30];

  sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
  put_fonts8_asc_sht(cons->sheet, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s,
                     30);
  cons_newline(cons);

  sprintf(s, "free %dKB", memman_total(memman) / 1024);
  put_fonts8_asc_sht(cons->sheet, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s,
                     30);
  cons_newline(cons);
  cons_newline(cons);
}

void cmd_cls(struct Console *cons) {
  struct Sheet *sheet = cons->sheet;

  for (int y = 28; y < 28 + 128; y++) {
    for (int x = 8; x < 8 + 240; x++) {
      sheet->buf[x + y * sheet->bxsize] = COL8_000000;
    }
  }

  sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
  cons->cur_y = 28;
}

void cmd_dir(struct Console *cons) {
  struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
  char s[30];

  for (int i = 0; i < 224; i++) {
    if (finfo[i].name[0] == '\0') {
      break;
    }

    if (finfo[i].name[0] != 0xe5) {
      if (!(finfo[i].type & 0x18)) {
        sprintf(s, "filename.ext   %7d", finfo[i].size);

        for (int j = 0; j < 8; j++) {
          s[j] = finfo[i].name[j];
        }
        s[9] = finfo[i].ext[0];
        s[10] = finfo[i].ext[1];
        s[11] = finfo[i].ext[2];

        put_fonts8_asc_sht(cons->sheet, 8, cons->cur_y, COL8_FFFFFF,
                           COL8_000000, s, 30);
        cons_newline(cons);
      }
    }
  }

  cons_newline(cons);
}

void cmd_type(struct Console *cons, int *fat, char *cmdline) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct FileInfo *finfo = file_search(
      cmdline + 5, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
  char *p;

  if (finfo) {
    p = (char *)memman_alloc_4k(memman, finfo->size);
    file_load_file(finfo->clustno, finfo->size, p, fat,
                   (char *)(ADR_DISKIMG + 0x003e00));
    for (int i = 0; i < finfo->size; i++) {
      cons_putchar(cons, p[i], 1);
    }
    memman_free_4k(memman, (int)p, finfo->size);
  } else {
    put_fonts8_asc_sht(cons->sheet, 8, cons->cur_y, COL8_FFFFFF, COL8_000000,
                       "File not found.", 15);
    cons_newline(cons);
  }

  cons_newline(cons);
}

void cmd_hlt(struct Console *cons, int *fat) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct FileInfo *finfo =
      file_search("HLT.HRB", (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
  char *p;

  if (finfo) {
    p = (char *)memman_alloc_4k(memman, finfo->size);
    file_load_file(finfo->clustno, finfo->size, p, fat,
                   (char *)(ADR_DISKIMG + 0x003e00));
    set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
    far_jmp(0, 1003 * 8);
    memman_free_4k(memman, (int)p, finfo->size);
  } else {
    put_fonts8_asc_sht(cons->sheet, 8, cons->cur_y, COL8_FFFFFF, COL8_000000,
                       "File not found.", 15);
    cons_newline(cons);
  }

  cons_newline(cons);
}
