#include "fs.h"

void file_read_fat(int *fat, unsigned char *img) {
  int i, j = 0;
  for (i = 0; i < 2880; i += 2) {
    fat[i + 0] = (img[j + 0] | img[j + 1] << 8) & 0xfff;
    fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
    j += 3;
  }
}

void file_load_file(int clustno, int size, char *buf, int *fat, char *img) {
  for (;;) {
    if (size <= 512) {
      for (int i = 0; i < size; i++) {
        buf[i] = img[clustno * 512 + i];
      }
      break;
    }
    for (int i = 0; i < 512; i++) {
      buf[i] = img[clustno * 512 + i];
    }

    size -= 512;
    buf += 512;
    clustno = fat[clustno];
  }
}
