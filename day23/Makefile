include ../Make.defines

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
		app/lines.hrb app/walk.hrb

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
	$(LD) $(APP_LDFLAGS) app/a.o app/api.o -o app/a.hrb

app/hello3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/hello3.c -o app/hello3.o
	$(LD) $(APP_LDFLAGS) app/hello3.o app/api.o -o app/hello3.hrb

app/crack1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/crack1.c -o app/crack1.o
	$(LD) $(APP_LDFLAGS) app/crack1.o app/api.o -o app/crack1.hrb

app/bug1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/bug1.c -fno-stack-protector -o app/bug1.o
	$(LD) $(APP_LDFLAGS) app/bug1.o app/api.o -o app/bug1.hrb

app/bug2.hrb:
	$(CC) $(APP_CFLAGS) -c app/bug2.c -o app/bug2.o
	$(LD) $(APP_LDFLAGS) app/bug2.o -o app/bug2.hrb

app/bug3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/bug3.c -o app/bug3.o
	$(LD) $(APP_LDFLAGS) app/bug3.o app/api.o -o app/bug3.hrb

app/bug4.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/bug4.c -o app/bug4.o
	$(LD) $(APP_LDFLAGS) app/bug4.o app/api.o -o app/bug4.hrb

app/hello5.hrb:
	$(AS) -f elf app/hello5.asm -o app/hello5.o
	$(LD) $(APP_LDFLAGS) app/hello5.o -o app/hello5.hrb

app/winhello.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello.c -o app/winhello.o
	$(LD) $(APP_LDFLAGS) app/winhello.o app/api.o -o app/winhello.hrb

app/winhelo2.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello2.c -o app/winhello2.o
	$(LD) $(APP_LDFLAGS) app/winhello2.o app/api.o -o app/winhelo2.hrb

app/winhelo3.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/winhello3.c -o app/winhello3.o
	$(LD) $(APP_LDFLAGS) app/winhello3.o app/api.o -o app/winhelo3.hrb

app/star1.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/star1.c -o app/star1.o
	$(LD) $(APP_LDFLAGS) app/star1.o app/api.o -o app/star1.hrb

app/stars.hrb: app/api.o $(LIBC_DIR)/stdlib/rand.bin
	$(CC) $(APP_CFLAGS) -I $(LIBC_DIR)/include -c app/stars.c -o app/stars.o
	$(LD) $(APP_LDFLAGS) app/stars.o app/api.o $(LIBC_DIR)/stdlib/rand.bin -o app/stars.hrb

app/stars2.hrb: app/api.o $(LIBC_DIR)/stdlib/rand.bin
	$(CC) $(APP_CFLAGS) -I $(LIBC_DIR)/include -c app/stars2.c -o app/stars2.o
	$(LD) $(APP_LDFLAGS) app/stars2.o app/api.o $(LIBC_DIR)/stdlib/rand.bin -o app/stars2.hrb

app/lines.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/lines.c -o app/lines.o
	$(LD) $(APP_LDFLAGS) app/lines.o app/api.o -o app/lines.hrb

app/walk.hrb: app/api.o
	$(CC) $(APP_CFLAGS) -c app/walk.c -o app/walk.o
	$(LD) $(APP_LDFLAGS) app/walk.o app/api.o -o app/walk.hrb

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
