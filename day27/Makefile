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
L_OBJS  := libc/stdio/stdio.bin libc/string/strcmp.bin libc/string/strncmp.bin \
		libc/stdlib/rand.bin
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
		app/beepup.hrb app/color.hrb app/color2.hrb app/crack7.hrb
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

# Applications
app/api.lib:
	$(AS) -f elf app/api.asm -o $@

%.hrb: %.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

app/%.o: app/%.c
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -I ./libc/include -c $< -o $@

app/%.hrb: app/%.o app/api.lib 
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/stars.hrb: app/stars.o app/api.lib libc/stdlib/rand.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/stars2.hrb: app/stars2.o app/api.lib libc/stdlib/rand.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/noodle.hrb: app/noodle.o app/api.lib libc/stdio/stdio.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

# Kernel
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
	rm -rf **/*.lib

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all
