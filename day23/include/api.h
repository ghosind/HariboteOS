#include "sheet.h"

#ifndef _API_H_
#define _API_H_

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
             int eax);

void asm_hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
                 int eax);

void api_line_win(struct Sheet *sht, int x0, int y0, int x1, int y1, int col);

#endif // _API_H_