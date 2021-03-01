all:
	nasm helloos.asm -o helloos.bin
	dd if=helloos.bin of=helloos.img bs=512

clean:
	rm -rf helloos.bin helloos.img

qemu: all
	qemu-system-i386 helloos.img

.PHONY:
	all
