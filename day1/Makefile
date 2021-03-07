all:
  nasm helloos.asm -o helloos.bin

clean:
  rm -rf helloos.bin

qemu: all
  qemu-system-i386 -fda helloos.bin

.PHONY:
  all
