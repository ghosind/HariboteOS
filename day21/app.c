#include "app.h"
#include "console.h"
#include "task.h"

int *int_handler0d(int *esp) {
  struct Console *cons = (struct Console *)*((int *)0x0fec);
  struct Task *task = task_now();

  cons_putstr(cons, "\nINT 0D:\n General Protected Exception.\n");

  return &(task->tss.esp0);
}
