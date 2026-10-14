sudo sh /home/pi/arbhar_v2/mountusb.sh

sudo rm /mnt/ramdisk/_preview/* 

SOURCE_DIR=$1
files=(
	"$SOURCE_DIR"/*.aif
	"$SOURCE_DIR"/*.wav
)

for i in "${files[@]}"
do
	#echo "${i##*/}"
	f=${i##*/}
	fileName=${f// /_}
       # echo $fileName


     sox -q -G "$i" -r 49148 /mnt/ramdisk/_preview/${fileName%%.*}_preview.wav trim 0 3 fade t 0.2 -0 1


done

echo "preview ready"

sudo sh /home/pi/arbhar_v2/mountusb.sh
