#include <stdio.h>

#include "memory.h"
#include "sheet.h"

struct Shtctl *shtctl_init(struct MemMan *memman, unsigned char *vram,
                           int xsize, int ysize) {
  struct Shtctl *ctl =
      (struct Shtctl *)memman_alloc_4k(memman, sizeof(struct Shtctl));
  if (!ctl) {
    return NULL;
  }

  ctl->map = (unsigned char *)memman_alloc_4k(memman, xsize * ysize);
  if (!ctl->map) {
    memman_free_4k(memman, (int)ctl, sizeof(struct Shtctl));
    return NULL;
  }

  ctl->vram = vram;
  ctl->xsize = xsize;
  ctl->ysize = ysize;
  ctl->top = -1; // 没有Sheet

  for (int i = 0; i < MAX_SHEETS; i++) {
    ctl->sheets0[i].flags = 0; // 标记为未使用
    ctl->sheets0[i].ctl = ctl; // 记录所属
  }

  return ctl;
}

struct Sheet *sheet_alloc(struct Shtctl *ctl) {
  struct Sheet *sht;
  for (int i = 0; i < MAX_SHEETS; i++) {
    if (ctl->sheets0[i].flags == 0) {
      sht = &ctl->sheets0[i];
      sht->flags = SHEET_USE; // 标记为正在使用
      sht->height = -1;       // 隐藏
      sht->task = 0;
      return sht;
    }
  }

  return NULL;
}

void sheet_setbuf(struct Sheet *sht, unsigned char *buf, int xsize, int ysize,
                  int col_inv) {
  sht->buf = buf;
  sht->bxsize = xsize;
  sht->bysize = ysize;
  sht->col_inv = col_inv;
}

void sheet_updown(struct Sheet *sht, int height) {
  struct Shtctl *ctl = sht->ctl;
  int h, old = sht->height; // 记录设置前的高度

  // 如果指定的高度过高或过低，则进行修正
  if (height > ctl->top + 1) {
    height = ctl->top + 1;
  }

  if (height < -1) {
    height = -1;
  }

  sht->height = height; // 设定高度

  // 下面主要是进行sheets[]的重新排列
  if (old > height) {
    // 比以前低
    if (height >= 0) {
      // 把中间的往上提
      for (h = old; h > height; h--) {
        ctl->sheets[h] = ctl->sheets[h - 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;

      sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, height + 1);
      sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, height + 1, old);
    } else {
      // 隐藏
      if (ctl->top > old) {
        // 把上面的降下来
        for (h = old; h < ctl->top; h++) {
          ctl->sheets[h] = ctl->sheets[h + 1];
          ctl->sheets[h]->height = h;
        }
      }
      ctl->top--; // 由于显示中的图层减少了一个，所以最上面的图层高度下降
      sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, 0);
      sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                       sht->vy0 + sht->bysize, 0, old - 1);
    }
  } else if (old < height) {
    // 比以前高
    if (old >= 0) {
      // 把中间的拉下去
      for (h = old; h < height; h++) {
        ctl->sheets[h] = ctl->sheets[h + 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;
    } else {
      // 由隐藏状态转为显示状态
      // 将已在上面的提上来
      for (h = ctl->top; h >= height; h--) {
        ctl->sheets[h + 1] = ctl->sheets[h];
        ctl->sheets[h + 1]->height = h + 1;
      }
      ctl->sheets[height] = sht;
      ctl->top++; // 由于已显示的图层增加了1个，所以最上面的图层高度增加
    }
    sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                     sht->vy0 + sht->bysize, height);
    sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
                     sht->vy0 + sht->bysize, height, height);
  }
}

void sheet_refreshmap(struct Shtctl *ctl, int vx0, int vy0, int vx1, int vy1,
                      int h0) {
  unsigned char *map = ctl->map;

  if (vx0 < 0) {
    vx0 = 0;
  }
  if (vy0 < 0) {
    vy0 = 0;
  }
  if (vx1 > ctl->xsize) {
    vx1 = ctl->xsize;
  }
  if (vy1 > ctl->ysize) {
    vy1 = ctl->ysize;
  }

  for (int h = h0; h <= ctl->top; h++) {
    struct Sheet *sht = ctl->sheets[h];
    unsigned char sid = sht - ctl->sheets0;
    unsigned char *buf = sht->buf;
    int bx0 = vx0 - sht->vx0;
    int by0 = vy0 - sht->vy0;
    int bx1 = vx1 - sht->vx0;
    int by1 = vy1 - sht->vy0;

    if (bx0 < 0) {
      bx0 = 0;
    }
    if (by0 < 0) {
      by0 = 0;
    }
    if (bx1 > sht->bxsize) {
      bx1 = sht->bxsize;
    }
    if (by1 > sht->bysize) {
      by1 = sht->bysize;
    }

    for (int by = by0; by < by1; by++) {
      int vy = sht->vy0 + by;
      for (int bx = bx0; bx < bx1; bx++) {
        int vx = sht->vx0 + bx;
        if (buf[by * sht->bxsize + bx] != sht->col_inv) {
          map[vy * ctl->xsize + vx] = sid;
        }
      }
    }
  }
}

void sheet_refreshsub(struct Shtctl *ctl, int vx0, int vy0, int vx1, int vy1,
                      int h0, int h1) {
  unsigned char *buf, *vram = ctl->vram, *map = ctl->map;
  struct Sheet *sht;

  if (vx0 < 0) {
    vx0 = 0;
  }
  if (vy0 < 0) {
    vy0 = 0;
  }
  if (vx1 > ctl->xsize) {
    vx1 = ctl->xsize;
  }
  if (vy1 > ctl->ysize) {
    vy1 = ctl->ysize;
  }

  for (int h = h0; h <= h1; h++) {
    sht = ctl->sheets[h];
    buf = sht->buf;
    unsigned char sid = sht - ctl->sheets0;

    // 使用vx0 ~ vy1，对bx0 ~ by1进行倒推
    int bx0 = vx0 - sht->vx0;
    int by0 = vy0 - sht->vy0;
    int bx1 = vx1 - sht->vx0;
    int by1 = vy1 - sht->vy0;

    if (bx0 < 0) {
      bx0 = 0;
    }
    if (by0 < 0) {
      by0 = 0;
    }
    if (bx1 > sht->bxsize) {
      bx1 = sht->bxsize;
    }
    if (by1 > sht->bysize) {
      by1 = sht->bysize;
    }

    for (int by = by0; by < by1; by++) {
      int vy = sht->vy0 + by;
      for (int bx = bx0; bx < bx1; bx++) {
        int vx = sht->vx0 + bx;
        if (map[vy * ctl->xsize + vx] == sid) {
          vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
        }
      }
    }
  }
}

void sheet_refresh(struct Sheet *sht, int bx0, int by0, int bx1, int by1) {
  if (sht->height >= 0) {
    // 如果正在显示，则按新图层的信息刷新画面
    sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1,
                     sht->vy0 + by1, sht->height, sht->height);
  }
}

void sheet_slide(struct Sheet *sht, int vx0, int vy0) {
  struct Shtctl *ctl = sht->ctl;
  int old_vx0 = sht->vx0, old_vy0 = sht->vy0;

  sht->vx0 = vx0;
  sht->vy0 = vy0;

  if (sht->height >= 0) {
    // 如果正在显示，则按新图层的信息刷新画面
    sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize,
                     old_vy0 + sht->bysize, 0);
    sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize,
                     sht->height);
    sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize,
                     old_vy0 + sht->bysize, 0, sht->height - 1);
    sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize,
                     sht->height, sht->height);
  }
}

void sheet_free(struct Sheet *sht) {
  if (sht->height >= 0) {
    sheet_updown(sht, -1); // 如果处于显示状态，则先设定为隐藏
  }

  sht->flags = 0; // 未使用标志
}
