#include "sheet.h"

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

struct Console {
  struct Sheet *sheet;
  int cur_x, cur_y, cur_c;
};

void console_task(struct Sheet *sheet, unsigned int memtotal);

void cons_putchar(struct Console *cons, int ch, char move);
void cons_newline(struct Console *cons);
void cons_run_cmd(char *cmdline, struct Console *cons, int *fat, unsigned int memtotal);

void asm_cons_putchar(void);

#endif // _CONSOLE_H_
