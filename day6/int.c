#include "bootpack.h"
#include "graphic.h"
#include "int.h"
#include "io.h"

void init_pic(void) {
  // 禁止所有中断
  io_out8(PIC0_IMR, 0xff);
  io_out8(PIC1_IMR, 0xff);

  io_out8(PIC0_ICW1, 0x11); // 边缘触发模式
  io_out8(PIC0_ICW2, 0x20); // IRQ0-7由INT20-27接收
  io_out8(PIC0_ICW3, 1 << 2); // PIC1由IRQ2连接
  io_out8(PIC0_ICW4, 0x01); // 无缓冲区模式

  io_out8(PIC1_ICW1, 0x11); // 边缘触发模式
  io_out8(PIC1_ICW2, 0x28); // IRQ8-15由INT28-2f接收
  io_out8(PIC1_ICW3, 2); // PIC1由IRQ2连接
  io_out8(PIC1_ICW4, 0x01); // 无缓冲区模式

  io_out8(PIC0_IMR, 0xfb); // PIC1以外中断全部禁止
  io_out8(PIC1_IMR, 0xff); // 禁止全部中断
}

void int_handler21(int *esp) {
  struct BootInfo *binfo = (struct BootInfo *) ADR_BOOTINFO;

  box_fill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 (IRQ-1) : PS/2 keyboard");

	for (;;) {
		io_hlt();
	}
}

void int_handler2c(int *esp) {
  struct BootInfo *binfo = (struct BootInfo *) ADR_BOOTINFO;

  box_fill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	put_fonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12) : PS/2 mouse");

	for (;;) {
		io_hlt();
	}
}

void int_handler27(int *esp) {
	io_out8(PIC0_OCW2, 0x67);

	return;
}
