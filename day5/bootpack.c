#include <stdio.h>

#include "io.h"
#include "pm.h"

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void box_fill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0,
               int x1, int y1);
void init_screen(unsigned char *vram, int x, int y);
void put_font8(unsigned char *vram, int xsize, int x, int y, char c,
               char *font);
void put_fonts8_asc(unsigned char *vram, int xsize, int x, int y, char c,
                    char *s);
void init_mouse_cursor8(char *mouse, char bc);
void put_block8_8(unsigned char *vram, int vxsize, int pxsize, int pysize,
                  int px0, int py0, char *buf, int bxsize);

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

struct BootInfo {
  char cyls, leds, vmode, reserve;
  short scrnx, scrny;
  unsigned char *vram;
};

struct SegmentDescriptor {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GateDescriptor {
  short offset_low, selector;
  char dw_count, access_right;
  short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SegmentDescriptor *sd, unsigned int limit, int base,
                  int ar);
void set_gatedesc(struct GateDescriptor *gd, int offset, int selector, int ar);

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *)0x0ff0;
  char s[40], mcursor[256];

  init_gdtidt();
  init_palette();

  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  int mx = (binfo->scrnx - 16) / 2;
  int my = (binfo->scrny - 28 - 16) / 2;
  init_mouse_cursor8(mcursor, COL8_008484);
  put_block8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  put_fonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "ABC 123");
  put_fonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000,
                 "Haribote OS.");
  put_fonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF,
                 "Haribote OS.");

  sprintf(s, "scrnx = %d", binfo->scrnx);
  put_fonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

  for (;;) {
    io_hlt();
  }

  return 0;
}

void init_palette(void) {
  static unsigned char table_rgb[16 * 3] = {
      0x00, 0x00, 0x00, // 黑色
      0xff, 0x00, 0x00, // 亮红色
      0x00, 0xff, 0x00, // 亮绿色
      0xff, 0xff, 0x00, // 亮黄色
      0x00, 0x00, 0xff, // 亮蓝色
      0xff, 0x00, 0xff, // 亮紫色
      0x00, 0xff, 0xff, // 浅亮蓝色
      0xff, 0xff, 0xff, // 白色
      0xc6, 0xc6, 0xc6, // 亮灰色
      0x84, 0x00, 0x00, // 暗红色
      0x00, 0x84, 0x00, // 暗绿色
      0x84, 0x84, 0x00, // 暗黄色
      0x00, 0x00, 0x84, // 暗蓝色
      0x84, 0x00, 0x84, // 暗紫色
      0x00, 0x84, 0x84, // 浅暗蓝色
      0x84, 0x84, 0x84  // 暗灰色
  };

  set_palette(0, 15, table_rgb);
}

void set_palette(int start, int end, unsigned char *rgb) {
  int eflags = io_load_eflags(); // 记录标志

  io_cli(); // 禁止中断

  io_out8(0x03c8, start);
  for (int i = start; i <= end; i++) {
    io_out8(0x03c9, rgb[0] / 4);
    io_out8(0x03c9, rgb[1] / 4);
    io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  }

  io_store_eflags(eflags);
}

void box_fill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0,
               int x1, int y1) {
  for (int y = y0; y <= y1; y++) {
    for (int x = x0; x <= x1; x++) {
      vram[y * xsize + x] = c;
    }
  }
}

void init_screen(unsigned char *vram, int x, int y) {
  box_fill8(vram, x, COL8_008484, 0, 0, x - 1, y - 29);
  box_fill8(vram, x, COL8_C6C6C6, 0, y - 28, x - 1, y - 28);
  box_fill8(vram, x, COL8_FFFFFF, 0, y - 27, x - 1, y - 27);
  box_fill8(vram, x, COL8_C6C6C6, 0, y - 26, x - 1, y - 1);

  box_fill8(vram, x, COL8_FFFFFF, 3, y - 24, 59, y - 24);
  box_fill8(vram, x, COL8_FFFFFF, 2, y - 24, 2, y - 4);
  box_fill8(vram, x, COL8_848484, 3, y - 4, 59, y - 4);
  box_fill8(vram, x, COL8_848484, 59, y - 23, 59, y - 5);
  box_fill8(vram, x, COL8_000000, 2, y - 4, 59, y - 3);
  box_fill8(vram, x, COL8_000000, 60, y - 24, 60, y - 3);

  box_fill8(vram, x, COL8_848484, x - 47, y - 24, x - 4, y - 24);
  box_fill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y - 4);
  box_fill8(vram, x, COL8_FFFFFF, x - 47, y - 3, x - 4, y - 3);
  box_fill8(vram, x, COL8_FFFFFF, x - 3, y - 24, x - 3, y - 3);
}

void put_font8(unsigned char *vram, int xsize, int x, int y, char c,
               char *font) {
  unsigned char *p;
  char d;

  for (int i = 0; i < 16; i++) {
    p = vram + (y + i) * xsize + x;
    d = font[i];

    if ((d & 0x80) != 0) {
      p[0] = c;
    }

    if ((d & 0x40) != 0) {
      p[1] = c;
    }

    if ((d & 0x20) != 0) {
      p[2] = c;
    }

    if ((d & 0x10) != 0) {
      p[3] = c;
    }

    if ((d & 0x08) != 0) {
      p[4] = c;
    }

    if ((d & 0x04) != 0) {
      p[5] = c;
    }

    if ((d & 0x02) != 0) {
      p[6] = c;
    }

    if ((d & 0x01) != 0) {
      p[7] = c;
    }
  }
}

void put_fonts8_asc(unsigned char *vram, int xsize, int x, int y, char c,
                    char *s) {
  extern char hankaku[4096];

  for (; *s != '\0'; s++) {
    put_font8(vram, xsize, x, y, c, hankaku + *s * 16);
    x += 8;
  }
}

void init_mouse_cursor8(char *mouse, char bc) {
  static char cursor[16][16] = {
      "**************..",
      "*OOOOOOOOOOO*...",
      "*OOOOOOOOOO*....",
      "*OOOOOOOOO*.....",
      "*OOOOOOOO*......",
      "*OOOOOOO*.......",
      "*OOOOOOO*.......",
      "*OOOOOOOO*......",
      "*OOOO**OOO*.....",
      "*OOO*..*OOO*....",
      "*OO*....*OOO*...",
      "*O*......*OOO*..",
      "**........*OOO*.",
      "*..........*OOO*",
      "............*OO*",
      ".............***"};

  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 16; x++) {
      if (cursor[y][x] == '*') {
        mouse[y * 16 + x] = COL8_000000;
      }

      if (cursor[y][x] == 'O') {
        mouse[y * 16 + x] = COL8_FFFFFF;
      }

      if (cursor[y][x] == '.') {
        mouse[y * 16 + x] = bc;
      }
    }
  }
}

void put_block8_8(unsigned char *vram, int vxsize, int pxsize, int pysize,
                  int px0, int py0, char *buf, int bxsize) {
  for (int y = 0; y < pysize; y++) {
    for (int x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
}

void init_gdtidt(void) {
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)0x00270000;
  struct GateDescriptor *idt = (struct GateDescriptor *)0x0026f800;

  for (int i = 0; i < 8192; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }

  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
  set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
  load_gdtr(0xffff, 0x00270000);

  for (int i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(0x7ff, 0x0026f800);
}

void set_segmdesc(struct SegmentDescriptor *sd, unsigned int limit, int base,
                  int ar) {
  if (limit > 0xfffff) {
    ar |= 0x8000; // G_bit = 1
    limit /= 0x1000;
  }

  sd->limit_low = limit & 0xffff;
  sd->base_low = base & 0xffff;
  sd->base_mid = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) | 0xf0);
  sd->base_high = (base >> 24) & 0xff;
}

void set_gatedesc(struct GateDescriptor *gd, int offset, int selector, int ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->dw_count = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high = (offset >> 16) & 0xffff;
}
