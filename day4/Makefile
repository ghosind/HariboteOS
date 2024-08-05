include ../Make.defines

OBJS	:= ipl.bin asmhead.bin bootpack.bin func.bin

ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin -l ipl.lst

asmhead.bin:
	$(AS) -f elf asmhead.asm -o asmhead.bin -l asmhead.lst

bootpack.bin:
	$(CC) $(CFLAGS) -c bootpack.c -o bootpack.bin

func.bin:
	$(AS) -f elf func.asm -o func.bin -l func.lst

haribote.sys: asmhead.bin bootpack.bin func.bin
	$(LD) -m elf_i386 --oformat binary asmhead.bin bootpack.bin func.bin -o haribote.sys -T haribote.ld

image: ipl.bin haribote.sys
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=haribote.sys of=$(IMG) seek=33 bs=512 conv=notrunc

all: ${OBJS} haribote.sys image

.PHONY:
	all
