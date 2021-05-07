#ifndef _APP_H_
#define _APP_H_

void start_app(int eip, int cs, int esp, int ds);

int int_handler0d(int *esp);

#endif // _APP_H_
