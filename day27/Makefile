include ../Make.defines

# Kernel Objects
K_OBJS	:= kernel/bootpack.bin kernel/io.bin kernel/pm.bin kernel/hankaku.bin \
		kernel/desctbl.bin kernel/graphic.bin kernel/int.bin \
		kernel/inthandler.bin kernel/fifo.bin kernel/keyboard.bin \
		kernel/mouse.bin kernel/memory.bin kernel/sheet.bin \
		kernel/window.bin kernel/timer.bin kernel/task_asm.bin \
		kernel/task.bin kernel/console.bin kernel/fs.bin \
		kernel/command.bin kernel/api.bin kernel/api_asm.bin \
		kernel/app.bin kernel/elf.bin
# Applications
APPS	:= app/hello.hrb app/hello2.hrb app/a.hrb app/hello3.hrb app/crack1.hrb \
		app/crack2.hrb app/crack3.hrb app/crack4.hrb app/crack5.hrb \
		app/crack6.hrb app/bug1.hrb app/bug2.hrb app/bug3.hrb \
		app/bug4.hrb app/hello5.hrb app/winhello.hrb app/winhelo2.hrb \
		app/winhelo3.hrb app/star1.hrb app/stars.hrb app/stars2.hrb \
		app/lines.hrb app/walk.hrb app/noodle.hrb app/beepdown.hrb \
		app/beepup.hrb app/color.hrb app/color2.hrb app/crack7.hrb

kernel/ipl.bin:
	$(AS) -f bin kernel/ipl.asm -o kernel/ipl.bin -l kernel/ipl.lst

kernel/asmhead.bin:
	$(AS) -f bin kernel/asmhead.asm -o kernel/asmhead.bin -l kernel/asmhead.lst

kernel/hankaku.bin: ../hankaku/hankaku.asm
	$(AS) -f elf $< -o $@

kernel/%.bin: kernel/%.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

%.bin: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Applications
app/api.lib:
	$(AS) -f elf app/api.asm -o $@

%.hrb: %.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

app/%.o: app/%.c
	$(CC) $(APP_CFLAGS) -I $(LIBC_DIR)/include -c $< -o $@

app/bug1.o:
	$(CC) $(APP_CFLAGS) -fno-stack-protector -c app/bug1.c -o app/bug1.o

app/noodle.o:
	$(CC) $(APP_CFLAGS) -fno-stack-protector -I $(LIBC_DIR)/include  -c app/noodle.c -o app/noodle.o

app/%.hrb: app/%.o app/api.lib
	$(LD) $(APP_LDFLAGS) $^ -o $@

app/stars.hrb: app/stars.o app/api.lib $(LIBC_DIR)/stdlib/rand.bin
	$(LD) $(APP_LDFLAGS) $^ -o $@

app/stars2.hrb: app/stars2.o app/api.lib $(LIBC_DIR)/stdlib/rand.bin
	$(LD) $(APP_LDFLAGS) $^ -o $@

app/noodle.hrb: app/noodle.o app/api.lib $(LIBC_DIR)/stdio/stdio.bin
	$(LD) $(APP_LDFLAGS) $^ -o $@

# Kernel
kernel/kernel.sys: ${K_OBJS} ${L_OBJS}
	$(LD) -m elf_i386 --oformat binary -o kernel/kernel.sys -T kernel/kernel.ld $^

haribote.sys: kernel/asmhead.bin kernel/kernel.sys
	cat kernel/asmhead.bin > haribote.sys
	cat kernel/kernel.sys >> haribote.sys

image: kernel/ipl.bin haribote.sys ${APPS}
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=kernel/ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	mcopy -i $(IMG) haribote.sys ::/
	mcopy -i $(IMG) ${APPS} ::/

all: ${L_OBJS} ${K_OBJS} ${APPS} haribote.sys image

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all
