#!/bash

leftFile=$2
rightFile=$3
stereoFile=$1

echo "split $stereoFile into $leftFile and $rightFile"

#sudo sox -b 32 -e floating-point -t raw -c 1 -r 49149 /mnt/ramdisk/$leftFile -b 32 -e floating-point -t raw -c 1 -r 49148 /mnt/ramdisk/$rightFile --channels 2 --combine merge -r 48k /home/pi/_autosave/$stereoFile
sudo sox /home/pi/_samples/$stereoFile -b 32 -e floating-point -c 1 -r 49149 -t raw /mnt/ramdisk/$leftFile remix 1 trim 0 13
sudo sox /home/pi/_samples/$stereoFile -b 32 -e floating-point -c 1 -r 49149 -t raw /mnt/ramdisk/$rightFile remix 2 trim 0 13
