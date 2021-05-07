#include "app.h"
#include "console.h"

int int_handler0d(int *esp) {
  struct Console *cons = (struct Console *)*((int *)0x0fec);
  cons_putstr(cons, "\nINT 0D:\n General Protected Exception.\n");
  return 1;
}
