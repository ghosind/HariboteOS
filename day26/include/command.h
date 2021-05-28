#include "console.h"

#ifndef _COMMAND_H_
#define _COMMAND_H_

void cmd_mem(struct Console *cons, unsigned int memtotal);
void cmd_cls(struct Console *cons);
void cmd_dir(struct Console *cons);
void cmd_type(struct Console *cons, int *fat, char *cmdline);
void cmd_exit(struct Console *cons, int *fat);

int cmd_app(struct Console *cons, int *fat, char *cmdline);

#endif // _COMMAND_H_
