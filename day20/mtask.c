#include <stdio.h>

#include "desctbl.h"
#include "io.h"
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

  for (int i = 0; i < MAX_TASKLEVELS; i++) {
    taskctl->level[i].running = 0;
    taskctl->level[i].now = 0;
  }

  struct Task *task = task_alloc();
  task->flags = 2;    // 活动中标志
  task->priority = 2; // 0.02s
  task->level = 0;    // 最高level
  task_add(task);
  task_switchsub();
  load_tr(task->sel);

  task_timer = timer_alloc();
  timer_set_timer(task_timer, task->priority);

	struct Task *idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);

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

void task_run(struct Task *task, int level, int priority) {
  if (level < 0) {
    level = task->level; // 不改变level
  }

  if (priority > 0) {
    task->priority = priority;
  }

  if (task->flags == 2 && task->level != level) {
    // 改变活动中的level
    task_remove(task); // flag会变为1
  }

  if (task->flags != 2) {
    task->level = level;
    task_add(task);
  }

  taskctl->lv_change = 1; // 下次任务切换时检查level
}

void task_switch(void) {
  struct TaskLevel *tl = &taskctl->level[taskctl->now_lv];
  struct Task *now_task = tl->tasks[tl->now];

  tl->now++;
  if (tl->now == tl->running) {
    tl->now = 0;
  }

  if (taskctl->lv_change) {
    task_switchsub();
    tl = &taskctl->level[taskctl->now_lv];
  }

  struct Task *new_task = tl->tasks[tl->now];
  timer_set_timer(task_timer, new_task->priority);
  if (new_task != now_task) {
    far_jmp(0, new_task->sel);
  }
}

void task_sleep(struct Task *task) {
  if (task->flags == 2) {
    // 如果任务处于唤醒状态
    struct Task *now_task = task_now();
    task_remove(task);
    if (task == now_task) {
      // 如果将休眠，则进行任务切换
      task_switchsub();
      now_task = task_now();
      far_jmp(0, now_task->sel);
    }
  }
}

struct Task *task_now(void) {
  struct TaskLevel *tl = &taskctl->level[taskctl->now_lv];
  return tl->tasks[tl->now];
}

void task_add(struct Task *task) {
  struct TaskLevel *tl = &taskctl->level[task->level];
  tl->tasks[tl->running] = task;
  tl->running++;
  task->flags = 2;
}

void task_remove(struct Task *task) {
  struct TaskLevel *tl = &taskctl->level[task->level];
  int i;

  for (i = 0; i < tl->running; i++) {
    if (tl->tasks[i] == task) {
      break;
    }
  }

  tl->running--;
  if (i < tl->now) {
    tl->now--; // 需要移动成员
  }

  if (tl->now >= tl->running) {
    tl->now = 0;
  }
  task->flags = 1; // 休眠中

  for (; i < tl->running; i++) {
    tl->tasks[i] = tl->tasks[i + 1];
  }
}

void task_switchsub(void) {
  int i;

  for (i = 0; i < MAX_TASKLEVELS; i++) {
    if (taskctl->level[i].running > 0) {
      break;
    }
  }

  taskctl->now_lv = i;
  taskctl->lv_change = 0;
}

void task_idle(void) {
  for (;;) {
    io_hlt();
  }
}
