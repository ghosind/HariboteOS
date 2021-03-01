all:
	nasm ipl.asm -o ipl.bin
	dd if=ipl.bin of=helloos.img bs=512
	# 使用0填充
	dd if=/dev/zero of=helloos.img seek=1 bs=512 count=2879

clean:
	rm -rf ipl.bin helloos.img

qemu: all
	qemu-system-i386 -fda helloos.img

.PHONY:
	all
