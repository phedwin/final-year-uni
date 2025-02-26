
ASFLAGS = -m32
LDFLAGS = -m elf_i386 -Ttext 0x7C00 --oformat binary

all: boot

boot: boot.o
	ld $(LDFLAGS) boot.o -o boot.bin

boot.o: bootx86/boot.S
	$(CC) -x assembler-with-cpp $(ASFLAGS) -c bootx86/boot.S -o boot.o

boot_img:
	dd if=/dev/zero of=boot.img bs=512 count=2880
	dd if=boot.bin of=boot.img conv=notrunc

debug:
	hexdump -C boot.bin > bootx86/MAGIC.txt

clean:
	rm -f *.o *.bin *.img bootx86/*.txt