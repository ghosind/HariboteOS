#ifndef _APP_H_
#define _APP_H_

void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);

#endif // _APP_H_
