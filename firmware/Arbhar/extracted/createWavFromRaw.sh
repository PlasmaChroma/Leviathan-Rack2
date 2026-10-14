


sudo sh mountusb.sh

for file in {0..5}
do
	echo "converting layer $file"
	sudo sox -r 49148 -t raw -b 32 -e floating-point /mnt/ramdisk/"$file"l_auto.raw -r 49148 -t raw -b 32 -e floating-point /mnt/ramdisk/"$file"r_auto.raw -r 48000 -c 2 /media/usb/"$file"_layer.wav -M
	sync
done
echo "export completed"
