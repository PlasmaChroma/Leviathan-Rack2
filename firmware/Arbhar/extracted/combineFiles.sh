sudo rm /mnt/ramdisk/_temp/aaaa.raw
touch /mnt/ramdisk/_temp/aaaa.raw

thresh=1850

SOURCE_DIR=/mnt/ramdisk/_temp

result=/mnt/ramdisk/_temp/aaaa.raw

files=(
	"$SOURCE_DIR"/*_M.raw
	"$SOURCE_DIR"/*_R.raw
)

for i in "${files[@]}"
do
	length=$(du $result | grep -o '^\S*')
	if [ $length -gt $thresh ]
	then
		exit 1
	fi
	sudo nice -n -7 sox -t raw -r 49148 -b 32 -c 1 -e floating-point $result -t raw -r 49148 -b 32 -c 1 -e floating-point "$i" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/combined.raw trim 0 13 &
	wait
	sudo nice -n -7 cp /mnt/ramdisk/_temp/combined.raw /mnt/ramdisk/_temp/aaaa.raw

done

sync
echo "all_combined"
