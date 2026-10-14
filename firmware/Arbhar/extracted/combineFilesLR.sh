

thresh=1850

SOURCE_DIR=/mnt/ramdisk/_temp/

result_L="$SOURCE_DIR"_LLLL.raw
result_R="$SOURCE_DIR"_RRRR.raw

touch "$result_L"
touch "$result_R"

files=(
	"$SOURCE_DIR"/*_L.raw
)

for i in "${files[@]}"
do
	length=$(du $result_L | grep -o '^\S*')
	echo "$i"
#	echo $length
	if [ $length -lt $thresh ]
	then
	sudo nice -n -7 sox -t raw -r 49148 -b 32 -c 1 -e floating-point $result_L -t raw -r 49148 -b 32 -c 1 -e floating-point "$i" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/combined_L.raw trim 0 13 &
	wait
	sudo nice -n -7 cp /mnt/ramdisk/_temp/combined_L.raw $result_L
	fi
done

sudo nice -n -7 mv $result_L /mnt/ramdisk/_temp/combined_L.raw &
wait

files_R=(
	"$SOURCE_DIR"/*_R.raw
)

for i in "${files_R[@]}"
do
	lengthR=$(du $result_R | grep -o '^\S*')
	echo "$i"
#	echo $length
	if [ $lengthR -lt $thresh ]
	then
	sudo nice -n -7 sox -t raw -r 49148 -b 32 -c 1 -e floating-point $result_R -t raw -r 49148 -b 32 -c 1 -e floating-point "$i" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/combined_R.raw trim 0 13 &
	wait
	sudo nice -n -7 mv /mnt/ramdisk/_temp/combined_R.raw $result_R
	fi
done

sudo nice -n -7 mv $result_R /mnt/ramdisk/_temp/combined_R.raw

echo "all_combined"

sudo rm /mnt/ramdisk/_temp/_LLLL.raw
sudo rm /mnt/ramdisk/_temp/_RRRR.raw

sync
#echo "sync from combineFilesLR"
