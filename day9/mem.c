#include "mem.h"
#include "io.h"

unsigned int memtest(unsigned int start, unsigned int end) {
  char flg486 = 0;
  unsigned int eflg, cr0;

  eflg = io_load_eflags();
  eflg |= EFLAGS_AC_BIT; // AC-bit = 1

  io_store_eflags(eflg);
  eflg = io_load_eflags();
  if ((eflg & EFLAGS_AC_BIT)) {
    // 如果是i386，即使设定AC=1，也会自动变回到0
    flg486 = 1;
  }

  eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
  io_store_eflags(eflg);

  if (flg486) {
    cr0 = io_load_cr0();
    cr0 |= CR0_CACHE_DISABLE; // 禁止缓存
    io_store_cr0(cr0);
  }

  unsigned int ret = memtest_sub(start, end);

  if (flg486) {
    cr0 = io_load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE; // 允许缓存
    io_store_cr0(cr0);
  }

  return ret;
}

unsigned int memtest_sub(unsigned int start, unsigned int end) {
  unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;

  for (i = start; i <= end; i += 0x1000) {
    p = (unsigned int *)(i + 0xffc);

    old = *p;         // 先记住修改前的值
    *p = pat0;        // 试写
    *p ^= 0xffffffff; // 反转

    if (*p != pat1) {
      // 检查反转结果
    not_memory:
      *p = old;
      break;
    }

    *p ^= 0xffffffff; // 再次反转
    if (*p != pat0) {
      // 检查值是否恢复
      goto not_memory;
    }
    *p = old;
  }

  return i;
}
