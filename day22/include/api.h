#ifndef _API_H_
#define _API_H_

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
             int eax);

void asm_hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx,
                 int eax);

#endif // _API_H_