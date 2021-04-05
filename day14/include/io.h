#ifndef _IO_H_
#define _IO_H_

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);

int io_in8(int port);
int io_in16(int port);
int io_in32(int port);

void io_out8(int port, int data);
void io_out16(int port, int data);
void io_out32(int port, int data);

int io_load_eflags(void);
void io_store_eflags(int eflags);

int io_load_cr0(void);
void io_store_cr0(int cr0);

#endif // _IO_H_
