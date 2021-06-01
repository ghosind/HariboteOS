#include <stdio.h>

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

struct FileInfo *file_search(char *name, struct FileInfo *finfo, int max) {
  int i, j;
  char s[12];

  for (j = 0; j < 11; j++) {
    s[j] = ' ';
  }
  j = 0;

  for (i = 0; name[i] != '\0'; i++) {
    if (j > 11) {
      return NULL;
    }

    if (name[i] == '.' && s[j] <= 'z') {
      j = 8;
    } else {
      s[j] = name[i];
      if ('a' <= s[j] && s[j] <= 'z') {
        // 转为大写
        s[j] -= 0x20;
      }
      j++;
    }
  }

  for (i = 0; i < max;) {
    if (finfo[i].name[0] == '\0') {
      break;
    }

    if (!(finfo[i].type & 0x18)) {
      for (j = 0; j < 11; j++) {
        if (finfo[i].name[j] != s[j]) {
          goto next;
        }
      }
      return finfo + i;
    }

  next:
    i++;
  }

  return NULL;
}
