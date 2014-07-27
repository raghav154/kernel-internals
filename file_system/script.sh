
make -f makefile.linux

sudo mount -o loop dev_kernel_grub.img /mnt/floppy

sudo cp kernel.bin /mnt/floppy/

sudo umount /mnt/floppy/

bochs -f bochsrc.bxrc

