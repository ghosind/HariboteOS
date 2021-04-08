#include <stdio.h>

#include "desctbl.h"
#include "memory.h"
#include "task.h"
#include "timer.h"

struct TaskCtl *taskctl;
struct Timer *task_timer;

struct Task *task_init(struct MemMan *memman) {
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
  taskctl = (struct TaskCtl *)memman_alloc_4k(memman, sizeof(struct TaskCtl));

  for (int i = 0; i < MAX_TASKS; i++) {
    taskctl->tasks0[i].flags = 0;
    taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
    set_segmdesc(gdt + TASK_GDT0 + i, 103, (int)&taskctl->tasks0[i].tss,
                 AR_TSS32);
  }

  struct Task *task = task_alloc();
  task->flags = 2; // 活动中标志
  taskctl->running = 1;
  taskctl->now = 0;
  taskctl->tasks[0] = task;
  load_tr(task->sel);

  task_timer = timer_alloc();
  timer_set_timer(task_timer, 2);

  return task;
}

struct Task *task_alloc(void) {
  struct Task *task;

  for (int i = 0; i < MAX_TASKS; i++) {
    if (taskctl->tasks0[i].flags == 0) {
      task = &taskctl->tasks0[i];

      task->flags = 1;               // 正在使用标志
      task->tss.eflags = 0x00000202; // IF = 1;
      task->tss.eax = 0;
      task->tss.ecx = 0;
      task->tss.edx = 0;
      task->tss.ebx = 0;
      task->tss.ebp = 0;
      task->tss.esi = 0;
      task->tss.edi = 0;
      task->tss.es = 0;
      task->tss.ds = 0;
      task->tss.fs = 0;
      task->tss.gs = 0;
      task->tss.ldtr = 0;
      task->tss.iomap = 0x40000000;

      return task;
    }
  }

  return NULL;
}

void task_run(struct Task *task) {
  task->flags = 2; // 活动中标志
  taskctl->tasks[taskctl->running] = task;
  taskctl->running++;
}

void task_switch(void) {
  timer_set_timer(task_timer, 2);
  if (taskctl->running >= 2) {
    taskctl->now++;
    if (taskctl->now == taskctl->running) {
      taskctl->now = 0;
    }

    far_jmp(0, taskctl->tasks[taskctl->now]->sel);
  }
}
