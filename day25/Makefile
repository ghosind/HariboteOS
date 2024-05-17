ifndef GCCPREFIX
GCCPREFIX :=
endif

AS	:= nasm
CC	:= $(GCCPREFIX)gcc
LD	:= $(GCCPREFIX)ld
OBJCOPY	:= $(GCCPREFIX)objcopy
QEMU	:= qemu-system-i386

LIBC_DIR := ../libc

CFLAGS += -Wall -Wno-format -Wno-unused
CFLAGS += -std=gnu99 -static -m32
CFLAGS += -I ./include -I $(LIBC_DIR)/include
CFLAGS += -ffunction-sections -nostdlib -nostdinc -fno-builtin -ffreestanding
CFLAGS += -fno-pie

APP_CFLAGS = -Wall -Wno-format -Wno-unused -std=gnu99 -fno-pie -m32

QEMU_FLAGS 	+= -no-reboot -d in_asm -m size=128

# C Library Objects
L_OBJS  := $(LIBC_DIR)/stdio/stdio.bin $(LIBC_DIR)/string/strcmp.bin \
		$(LIBC_DIR)/string/strncmp.bin $(LIBC_DIR)/stdlib/rand.bin
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
		app/bug4.hrb app/hello5.hrb app/winhello.hrb app/winhelo2.hrb \
		app/winhelo3.hrb app/star1.hrb app/stars.hrb app/stars2.hrb \
		app/lines.hrb app/walk.hrb app/noodle.hrb app/beepdown.hrb \
		app/beepup.hrb app/color.hrb app/color2.hrb
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

hankaku.bin: ../hankaku/hankaku.asm
	$(AS) -f elf $< -o $@

%.bin: %.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.hrb: %.asm
	$(AS) -f bin $< -o $@  -l $(subst .asm,.lst,$<)

app/api.o:
	$(AS) -f elf app/api.asm -o app/api.o

app/a.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/a.c -o app/a.o
	$(LD) -m elf_i386 --oformat binary -e main app/a.o app/api.o -o app/a.hrb

app/hello3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/hello3.c -o app/hello3.o
	$(LD) -m elf_i386 --oformat binary -e main app/hello3.o app/api.o -o app/hello3.hrb

app/crack1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/crack1.c -o app/crack1.o
	$(LD) -m elf_i386 --oformat binary -e main app/crack1.o app/api.o -o app/crack1.hrb

app/bug1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -fno-stack-protector -c app/bug1.c -o app/bug1.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug1.o app/api.o -o app/bug1.hrb

app/bug2.hrb:
	$(CC) $(APP_CFLAGS) -c app/bug2.c -o app/bug2.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug2.o -o app/bug2.hrb

app/bug3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/bug3.c -o app/bug3.o
	$(LD) -m elf_i386 --oformat binary -e main app/bug3.o app/api.o -o app/bug3.hrb

app/bug4.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/bug4.c -o app/bug4.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/bug4.o app/api.o -o app/bug4.hrb

app/hello5.hrb:
	$(AS) -f elf app/hello5.asm -o app/hello5.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/hello5.o -o app/hello5.hrb

app/winhello.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello.c -o app/winhello.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/winhello.o app/api.o -o app/winhello.hrb

app/winhelo2.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello2.c -o app/winhello2.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/winhello2.o app/api.o -o app/winhelo2.hrb

app/winhelo3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello3.c -o app/winhello3.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/winhello3.o app/api.o -o app/winhelo3.hrb

app/star1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/star1.c -o app/star1.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/star1.o app/api.o -o app/star1.hrb

app/stars.hrb: app/api.o $(LIBC_DIR)/stdlib/rand.bin
	$(CC) $(APP_CFLAGS) -c -I $(LIBC_DIR)/include app/stars.c -o app/stars.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/stars.o app/api.o $(LIBC_DIR)/stdlib/rand.bin -o app/stars.hrb

app/stars2.hrb: app/api.o $(LIBC_DIR)/stdlib/rand.bin
	$(CC) $(APP_CFLAGS) -I $(LIBC_DIR)/include -c app/stars2.c -o app/stars2.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/stars2.o app/api.o $(LIBC_DIR)/stdlib/rand.bin -o app/stars2.hrb

app/lines.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/lines.c -o app/lines.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/lines.o app/api.o -o app/lines.hrb

app/walk.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/walk.c -o app/walk.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/walk.o app/api.o -o app/walk.hrb

app/noodle.hrb: app/api.o $(LIBC_DIR)/stdio/stdio.bin
	$(CC) $(APP_CFLAGS) -I $(LIBC_DIR)/include -fno-stack-protector -c app/noodle.c -o app/noodle.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/noodle.o app/api.o $(LIBC_DIR)/stdio/stdio.bin -o app/noodle.hrb

app/beepdown.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/beepdown.c -o app/beepdown.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/beepdown.o app/api.o -o app/beepdown.hrb

app/beepup.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/beepup.c -o app/beepup.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/beepup.o app/api.o -o app/beepup.hrb

app/color.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/color.c -o app/color.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/color.o app/api.o -o app/color.hrb

app/color2.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/color2.c -o app/color2.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/color2.o app/api.o -o app/color2.hrb

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
	rm -rf $(LIBC_DIR)/**/*.bin

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all
