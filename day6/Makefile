ifndef GCCPREFIX
GCCPREFIX :=
endif

AS   		:= nasm
CC   		:= $(GCCPREFIX)gcc
LD			:= $(GCCPREFIX)ld
OBJCOPY := $(GCCPREFIX)objcopy
CFLAGS	:= -Wall -m32 -I ./include
QEMU    := qemu-system-i386

K_OBJS  := asmhead.bin bootpack.bin io.bin pm.bin hankaku.bin desctbl.bin graphic.bin
SYS  		:= haribote.sys
IMG  		:= haribote.img

QEMU_FLAGS :=

ifdef DEBUG
QEMU_FLAGS += -gdb tcp::1234 -S
CFLAGS += -g

endif

ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin -l ipl.lst

%.bin: %.asm
	$(AS) -f elf $< -o $@

%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

haribote.sys: ${K_OBJS}
	$(LD) -m elf_i386 --oformat binary -o haribote.sys -T haribote.ld $^

image: ipl.bin haribote.sys
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=haribote.sys of=$(IMG) seek=33 bs=512 conv=notrunc

all: ${OBJS} haribote.sys image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf *.obj
	rm -rf *.lst
	rm -rf $(IMG)

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all
