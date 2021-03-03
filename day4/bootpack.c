void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
// void write_mem8(int addr, int data);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

// 入口函数HariMain重命名为标准的main
void main(void) {
  char *p = (char *) 0xa0000;

  init_palette();

  for (int i = 0; i <= 0xffff; i++) {
    // write_mem8(i, i & 0x0f);
    *(p + i) = i & 0x0f;
  }

  for (;;) {
    io_hlt();
  }
}

void init_palette(void) {
  static unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00,
    0xff, 0x00, 0x00,
    0x00, 0xff, 0x00,
    0xff, 0xff, 0x00,
    0x00, 0x00, 0xff,
    0xff, 0x00, 0xff,
    0x00, 0xff, 0xff,
    0xff, 0xff, 0xff,
    0xc6, 0xc6, 0xc6,
    0x84, 0x00, 0x00,
    0x00, 0x84, 0x00,
    0x84, 0x84, 0x00,
    0x00, 0x00, 0x84,
    0x84, 0x00, 0x84,
    0x00, 0x84, 0x84,
    0x84, 0x84, 0x84
  };
  set_palette(0, 15, table_rgb);

  return;
}

void set_palette(int start, int end, unsigned char *rgb) {
  int i, eflags;

  eflags = io_load_eflags();
  io_cli();
  io_out8(0x03c8, start);

  for (i = start; i <= end; i++) {
    io_out8(0x03c9, rgb[0] / 4);
    io_out8(0x03c9, rgb[1] / 4);
    io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  }

  io_store_eflags(eflags);

  return;
}
