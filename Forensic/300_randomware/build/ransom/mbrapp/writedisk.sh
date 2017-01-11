nbdloaded=`lsmod | grep nbd | wc -l`

if [ $nbdloaded -eq 0 ]; then
	modprobe nbd
fi

qemu-nbd -c /dev/nbd0 disk.img
dd if=./mbr of=/dev/nbd0
qemu-nbd -d /dev/nbd0
