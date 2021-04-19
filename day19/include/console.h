#include "sheet.h"

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

void console_task(struct Sheet *sheet, unsigned int memtotal);

int cons_newline(int cursor_y, struct Sheet *sheet);

#endif // _CONSOLE_H_
