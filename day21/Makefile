include ../Make.defines

# Kernel Objects
K_OBJS	:= bootpack.bin io.bin pm.bin hankaku.bin desctbl.bin graphic.bin \
		int.bin inthandler.bin fifo.bin keyboard.bin mouse.bin \
		memory.bin sheet.bin window.bin timer.bin task_asm.bin \
		task.bin console.bin fs.bin command.bin api.bin api_asm.bin \
		app.bin
# Applications
APPS	:= app/hello.hrb app/hello2.hrb app/a.hrb app/hello3.hrb app/crack1.hrb \
		app/crack2.hrb

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

app/a_asm.o:
	$(AS) -f elf app/a_asm.asm -o app/a_asm.o

app/a.hrb: app/a_asm.o
	$(CC) $(APP_CFLAGS) -c app/a.c -o app/a.o
	$(LD) $(APP_LDFLAGS) app/a.o app/a_asm.o -o app/a.hrb

app/hello3.hrb: app/a_asm.o
	$(CC) $(APP_CFLAGS) -c app/hello3.c -o app/hello3.o
	$(LD) $(APP_LDFLAGS) app/hello3.o app/a_asm.o -o app/hello3.hrb

app/crack1.hrb: app/a_asm.o
	$(CC) $(APP_CFLAGS) -c app/crack1.c -o app/crack1.o
	$(LD) $(APP_LDFLAGS) app/crack1.o app/a_asm.o -o app/crack1.hrb

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

.PHONY:
	all
