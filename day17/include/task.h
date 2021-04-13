#ifndef _TASK_H_
#define _TASK_H_

#include "memory.h"
#include "timer.h"

#define MAX_TASKS 1000 // 最大任务数量
#define TASK_GDT0 3    // 定义从GDT的几号开始分配给TSS

#define MAX_TASKS_LV 100
#define MAX_TASKLEVELS 10

struct TSS32 {
  int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
  int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
  int es, cs, ss, ds, fs, gs;
  int ldtr, iomap;
};

struct Task {
  int sel, flags; // sel用于存放GDT的编号
  int level, priority;
  struct TSS32 tss;
};

struct TaskLevel {
  int running; // 正在运行的任务数量
  int now;     // 记录正在运行的任务
  struct Task *tasks[MAX_TASKS_LV];
};

struct TaskCtl {
  int now_lv;    // 现在活动中的level
  int lv_change; // 在下次任务切换时是否需要改变level
  struct TaskLevel level[MAX_TASKLEVELS];
  struct Task tasks0[MAX_TASKS];
};

extern struct TaskCtl *taskctl;
extern struct Timer *task_timer;

void load_tr(int tr);
void far_jmp(int eip, int cs);

struct Task *task_init(struct MemMan *memman);
struct Task *task_alloc(void);
void task_run(struct Task *task, int level, int priority);
void task_switch(void);
void task_sleep(struct Task *task);
struct Task *task_now(void);
void task_add(struct Task *task);
void task_remove(struct Task *task);
void task_switchsub(void);
void task_idle(void);

#endif // _TASK_H_
