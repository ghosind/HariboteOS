#include <stdio.h>

#include "api.h"
#include "console.h"
#include "graphic.h"
#include "sheet.h"
#include "task.h"
#include "window.h"

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
             int eax) {
  int ds_base = *((int *)0x0fe8);
  struct Task *task = task_now();
  struct Console *cons = (struct Console *)*((int *)0x0fec);
  struct Shtctl *shtctl = (struct Shtctl *)*((int *)0x0fe4);
  struct Sheet *sht;
  char s[12];
  int *reg = &eax + 1;

  if (edx == 1) {
    cons_putchar(cons, eax & 0xff, 1);
  } else if (edx == 2) {
    cons_putstr(cons, (char *)ebx + ds_base);
  } else if (edx == 3) {
    cons_putnstr(cons, (char *)ebx + ds_base, ecx);
  } else if (edx == 4) {
    return &(task->tss.esp0);
  } else if (edx == 5) {
    sht = sheet_alloc(shtctl);
    sheet_setbuf(sht, (unsigned char *)(ebx + ds_base), esi, edi, eax);
    make_window8((unsigned char *)(ebx + ds_base), esi, edi,
                 (char *)(ecx + ds_base), 0);
    sheet_slide(sht, 100, 50);
    sheet_updown(sht, 3);
    reg[7] = (int)sht;
  } else if (edx == 6) {
    sht = (struct Sheet *)ebx;
    put_fonts8_asc(sht->buf, sht->bxsize, esi, edi, eax,
                   (char *)(ebp + ds_base));
    sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
  } else if (edx == 7) {
    sht = (struct Sheet *)ebx;
    box_fill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
    sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
  } else if (edx == 8) {
    memman_init((struct MemMan *)(ebx + ds_base));
    ecx &= 0xfffffff0;
    memman_free((struct MemMan *)(ebx + ds_base), eax, ecx);
  } else if (edx == 9) {
    ecx = (ecx + 0x0f) & 0xfffffff0;
    reg[7] = memman_alloc((struct MemMan *)(ebx + ds_base), ecx);
  } else if (edx == 10) {
    ecx = (ecx + 0x0f) & 0xfffffff0;
    memman_free((struct MemMan *)(ebx + ds_base), eax, ecx);
  } else if (edx == 11) {
    sht = (struct Sheet *) ebx;
    sht->buf[sht->bxsize * edi + esi] = eax;
    sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
  }

  // } else if (edx == 123456789) {
  //   *((char *) 0x00102600) = 0;
  // }

  return 0;
}
