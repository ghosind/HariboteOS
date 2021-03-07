#ifndef _BOOTPACK_H_
#define _BOOTPACK_H_

struct BootInfo{
  char          cyls, leds, vmode, reserve;
  short         scrnx, scrny;
  unsigned char *vram;
};

#endif // _BOOTPACK_H_
