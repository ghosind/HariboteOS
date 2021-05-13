ifndef GCCPREFIX
GCCPREFIX :=
endif

AS	:= nasm
CC	:= $(GCCPREFIX)gcc
LD	:= $(GCCPREFIX)ld
OBJCOPY	:= $(GCCPREFIX)objcopy
QEMU	:= qemu-system-i386

CFLAGS += -Wall -Wno-format -Wno-unused
CFLAGS += -std=gnu99 -static -m32
CFLAGS += -I ./include -I ./libc/include
CFLAGS += -ffunction-sections -nostdlib -nostdinc -fno-builtin -ffreestanding

QEMU_FLAGS 	+= -no-reboot -d in_asm -m size=128

# C Library Objects
L_OBJS  := libc/stdio/stdio.bin libc/string/strcmp.bin libc/string/strncmp.bin
# Kernel Objects
K_OBJS	:= bootpack.bin io.bin pm.bin hankaku.bin desctbl.bin graphic.bin \
		int.bin inthandler.bin fifo.bin keyboard.bin mouse.bin \
		memory.bin sheet.bin window.bin timer.bin task_asm.bin \
		task.bin console.bin fs.bin command.bin api.bin api_asm.bin \
		app.bin elf.bin
# Applications
APPS	:= app/hello.hrb app/hello2.hrb app/a.hrb app/hello3.hrb app/crack1.hrb \
		app/crack2.hrb app/crack3.hrb app/crack4.hrb app/crack5.hrb \
		app/crack6.hrb app/bug1.hrb app/bug2.hrb app/bug3.hrb \
		app/bug4.hrb
SYS	:= haribote.sys
IMG	:= haribote.img

ifdef DEBUG
QEMU_FLAGS += -gdb tcp::1234 -S
CFLAGS += -g
endif

ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin -l ipl.lst

asmhead.bin:
	$(AS) -f bin asmhead.asm -o asmhead.bin -l asmhead.lst

%.bin: %.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.hrb: %.asm
	$(AS) -f bin $< -o $@  -l $(subst .asm,.lst,$<)

app/api.o:
	$(AS) -f elf app/api.asm -o app/api.o

app/a.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/a.c -o app/a.o
	$(LD) -m elf_i386 --oformat binary -e main app/a.o app/api.o -o app/a.hrb

app/hello3.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/hello3.c -o app/hello3.o
	$(LD) -m elf_i386 --oformat binary -e main app/hello3.o app/api.o -o app/hello3.hrb

app/crack1.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/crack1.c -o app/crack1.o
	$(LD) -m elf_i386 --oformat binary -e main app/crack1.o app/api.o -o app/crack1.hrb

app/bug1.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/bug1.c -o app/bug1.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug1.o app/api.o -o app/bug1.hrb

app/bug2.hrb:
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/bug2.c -o app/bug2.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug2.o -o app/bug2.hrb

app/bug3.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/bug3.c -o app/bug3.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug3.o app/api.o -o app/bug3.hrb

app/bug4.hrb: app/api.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/bug4.c -o app/bug4.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/bug4.o app/api.o -o app/bug4.hrb

kernel.sys: ${K_OBJS} ${L_OBJS}
	$(LD) -m elf_i386 --oformat binary -o kernel.sys -T kernel.ld $^

haribote.sys: asmhead.bin kernel.sys
	cat asmhead.bin > haribote.sys
	cat kernel.sys >> haribote.sys

image: ipl.bin haribote.sys ${APPS}
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	mcopy -i $(IMG) haribote.sys ::/
	mcopy -i $(IMG) ${APPS} ::/

all: ${L_OBJS} ${K_OBJS} ${APPS} haribote.sys image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf *.obj
	rm -rf *.lst
	rm -rf $(IMG)
	rm -rf **/**/*.bin
	rm -rf app/*.hrb
	rm -rf **/*.o
	rm -rf **/*.lst

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all
