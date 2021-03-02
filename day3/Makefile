AS   		:= nasm
CC   		:= x86_64-elf-gcc
LD			:= x86_64-elf-ld
OBJCOPY := x86_64-elf-objcopy
CFLAGS	:= -Wall -m32

SYS  		:= haribote.sys
IMG  		:= haribote.img

ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin

asmhead.bin:
	$(AS) -f bin asmhead.asm -o asmhead.bin

bootpack.bin:
	$(CC) $(CFLAGS) -m32 -c bootpack.c -o bootpack.bin

func.bin:
	$(AS) -f elf func.asm -o func.bin

haribote.sys: asmhead.bin bootpack.bin func.bin
	x86_64-elf-ld -m elf_i386 bootpack.bin func.bin -o bootpack.obj
	x86_64-elf-objcopy -S -O binary bootpack.obj bootpack.sys
	cat asmhead.bin > haribote.sys
	cat bootpack.sys >> haribote.sys

image: ipl.bin haribote.sys
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=haribote.sys of=$(IMG) bs=512 conv=notrunc
	
all: image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf *.obj
	rm -rf $(IMG)

qemu: all
	qemu-system-i386 -fda $(IMG)

.PHONY:
	all
