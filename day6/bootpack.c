#include "bootpack.h"
#include "desctbl.h"
#include "graphic.h"
#include "io.h"

int main(void) {
  struct BootInfo *binfo = (struct BootInfo *) 0x0ff0;
  char mcursor[256];
  
  init_gdtidt();
  init_palette();

  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
  
  int mx = (binfo->scrnx - 16) / 2;
	int my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	put_block8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  for (;;) {
    io_hlt();
  }

  return 0;
}
