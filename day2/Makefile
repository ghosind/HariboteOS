all:
	nasm helloos.asm -o helloos.bin -l helloos.lst
	dd if=/dev/zero of=helloos.img bs=512 count=2880
	dd if=helloos.bin of=helloos.img bs=512 count=1 conv=notrunc

clean:
	rm -rf helloos.bin helloos.img

qemu: all
	qemu-system-i386 -fda helloos.img -gdb tcp::1234 -S

.PHONY:
	all
