#ifndef _TASK_H_
#define _TASK_H_

#include "memory.h"
#include "timer.h"

#define MAX_TASKS 1000 // 最大任务数量
#define TASK_GDT0 3    // 定义从GDT的几号开始分配给TSS

struct TSS32 {
  int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  int es, cs, ss, ds, fs, gs;
  int ldtr, iomap;
};

struct Task {
  int sel, flags; // sel用于存放GDT的编号
  struct TSS32 tss;
};

struct TaskCtl {
  int running; // 正在运行的任务数量
  int now;     // 记录正在运行的任务
  struct Task *tasks[MAX_TASKS];
  struct Task tasks0[MAX_TASKS];
};

extern struct TaskCtl *taskctl;
extern struct Timer *task_timer;

void load_tr(int tr);
void far_jmp(int eip, int cs);

struct Task *task_init(struct MemMan *memman);
struct Task *task_alloc(void);
void task_run(struct Task *task);
void task_switch(void);
void task_sleep(struct Task *task);

#endif // _TASK_H_
