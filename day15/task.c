#include "task.h"
#include "timer.h"

struct Timer *mt_timer;
int mt_tr;

void mt_init(void) {
  mt_timer = timer_alloc();
  // 没有必要使用timer_init
  timer_set_timer(mt_timer, 2);
  mt_tr = 3 * 8;
}

void mt_task_switch(void) {
  if (mt_tr == 3 * 8) {
    mt_tr = 4 * 8;
  } else {
    mt_tr = 3 * 8;
  }

  timer_set_timer(mt_timer, 2);
  far_jmp(0, mt_tr);
}
