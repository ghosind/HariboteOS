void io_hlt(void);

// 入口函数HariMain重命名为标准的main
void main(void) {
fin:
  io_hlt();
  goto fin;
}
