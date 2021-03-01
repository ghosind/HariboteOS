all:
	nasm helloos.asm -o helloos.bin

clean:
	rm -rf helloos.bin

qemu: all
	qemu-system-i386 helloos.bin

.PHONY:
	all
