#!/bash

leftFile=$1
rightFile=$2
stereoFile=$3

echo "combine $leftFile and $rightFile into $stereoFile"

sudo sox -b 32 -e floating-point -t raw -c 1 -r 49205 /mnt/ramdisk/$leftFile -b 32 -e floating-point -t raw -c 1 -r 49205 /mnt/ramdisk/$rightFile --channels 2 --combine merge -r 48k /home/pi/_autosave/$stereoFile