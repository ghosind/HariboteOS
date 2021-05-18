#include <stdio.h>

#include "app.h"
#include "bootpack.h"
#include "command.h"
#include "console.h"
#include "desctbl.h"
#include "elf.h"
#include "fs.h"
#include "graphic.h"
#include "memory.h"
#include "sheet.h"
#include "task.h"

void cmd_mem(struct Console *cons, unsigned int memtotal) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  char s[30];

  sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024),
          memman_total(memman) / 1024);
  cons_putstr(cons, s);
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
        sprintf(s, "filename.ext   %7d\n", finfo[i].size);

        for (int j = 0; j < 8; j++) {
          s[j] = finfo[i].name[j];
        }
        s[9] = finfo[i].ext[0];
        s[10] = finfo[i].ext[1];
        s[11] = finfo[i].ext[2];

        cons_putstr(cons, s);
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
    cons_putnstr(cons, p, finfo->size);
    memman_free_4k(memman, (int)p, finfo->size);
  } else {
    cons_putstr(cons, "File not found.\n");
  }

  cons_newline(cons);
}

int cmd_app(struct Console *cons, int *fat, char *cmdline) {
  struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
  struct FileInfo *finfo;
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
  struct Task *task = task_now();
  char name[18];
  int i;

  for (i = 0; i < 13; i++) {
    if (cmdline[i] <= ' ') {
      break;
    }

    name[i] = cmdline[i];
  }
  name[i] = '\0';

  finfo = file_search(name, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
  if (finfo == NULL && name[i - 1] != '.') {
    name[i] = '.';
    name[i + 1] = 'H';
    name[i + 2] = 'R';
    name[i + 3] = 'B';
    name[i + 4] = '\0';

    finfo = file_search(name, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
  }

  if (finfo) {
    char *p = (char *)memman_alloc_4k(memman, finfo->size);
    *((int *)0x0fe8) = (int)p;
    file_load_file(finfo->clustno, finfo->size, p, fat,
                   (char *)(ADR_DISKIMG + 0x003e00));
    Elf32_Ehdr *elfhdr = (Elf32_Ehdr *)p;

    if (elf32_validate(elfhdr)) {
      char *q = (char *)memman_alloc_4k(memman, 64 * 1024);
      *((int *)0x0fe8) = (int)q;

      set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER + 0x60);
      set_segmdesc(gdt + 1004, 64 * 1024 - 1, (int)q, AR_DATA32_RW + 0x60);

      for (int i = 0; i < elfhdr->e_shnum; i++) {
        Elf32_Shdr *shdr =
            (Elf32_Shdr *)(p + elfhdr->e_shoff + sizeof(Elf32_Shdr) * i);

        if (shdr->sh_type != SHT_PROGBITS || !(shdr->sh_flags & SHF_ALLOC)) {
          continue;
        }

        for (int i = 0; i < shdr->sh_size; i++) {
          q[shdr->sh_addr + i] = p[shdr->sh_offset + i];
        }
      }

      start_app(elfhdr->e_entry, 1003 * 8, 0, 1004 * 8, &(task->tss.esp0));
      memman_free_4k(memman, (int)q, 64 * 1024);
    } else {
      cons_putstr(cons, "ELF file format error.\n");
    }

    memman_free_4k(memman, (int)p, finfo->size);
    cons_newline(cons);

    return 1;
  }

  return 0;
}
