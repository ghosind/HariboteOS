#ifndef _BOOTPACK_H_
#define _BOOTPACK_H_

#define ADR_BOOTINFO 0x00000ff0

struct BootInfo{
  char  cyls;
  char  leds;
  char  vmode;
  char  reserve;
  short scrnx;
  short scrny;
  unsigned char *vram;
};

#endif // _BOOTPACK_H_
