void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
// void write_mem8(int addr, int data);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

// 入口函数HariMain重命名为标准的main
// 返回类型修改为int，避免编译器警告
int main(void) {
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
  // 修改后无法正常读取到静态变量，所以将static移除
  // static unsigned char table_rgb[16 * 3] = {
  unsigned char table_rgb[16 * 3] = {
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

  return;
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

  return;
}
