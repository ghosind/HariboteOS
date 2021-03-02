AS   		:= nasm
CC   		:= x86_64-elf-gcc
CFLAGS	:= -Wall -m32

AS_OBJS := ipl.bin asmhead.bin func.bin
C_OBJS  := bootpack.bin
OBJS    := ${AS_OBJS} ${C_OBJS}

SYS  		:= haribote.sys
IMG  		:= haribote.img

%.bin: %.asm
	$(AS) $< -o $@

%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

haribote.sys:
	cat asmhead.bin bootpack.bin > haribote.sys

image:
	dd if=ipl.bin of=$(IMG) bs=512
	# 0x2600写入文件名，此处忽略
	# 软盘在0x4200（扇区34）以后存放文件内容，使用0填充填充扇区1～33
	dd if=/dev/zero of=$(IMG) seek=1 bs=512 count=31
	# 写入系统内容
	dd if=haribote.sys of=$(IMG) seek=33 bs=512
	# 使用0填充
	dd if=/dev/zero of=$(IMG) seek=34 bs=512 count=2846

all: ${OBJS} haribote.sys image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf $(IMG)

qemu: all
	qemu-system-i386 -fda $(IMG)

.PHONY:
	all
