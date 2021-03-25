#ifndef _TIMER_H_
#define _TIMER_H_

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TimerCtl {
  unsigned int count;
};

extern struct TimerCtl timerctl;

void init_pit(void);

void int_handler20(int *esp);

#endif // _TIMER_H_
