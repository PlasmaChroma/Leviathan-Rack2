
file=$1

sudo sox -n -r 49148 -t raw -b 32 -e floating-point /mnt/ramdisk/"$file"l_auto.raw trim 0 13

sudo sox -n -r 49148 -t raw -b 32 -e floating-point /mnt/ramdisk/"$file"r_auto.raw trim 0 13

sync
