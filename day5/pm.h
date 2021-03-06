#ifndef _PM_H_
#define _PM_H_

void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

#endif // _PM_H_
