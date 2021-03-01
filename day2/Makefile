all:
	nasm helloos.asm -o helloos.bin
	dd if=helloos.bin of=helloos.img bs=512
	# 填充
	dd if=/dev/zero of=helloos.img seek=1 bs=512 count=2879

clean:
	rm -rf helloos.bin helloos.img

qemu: all
	qemu-system-i386 -fda helloos.img

.PHONY:
	all
