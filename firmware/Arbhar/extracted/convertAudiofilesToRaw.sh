#!/bin/bash
#START=$(date +%s)
#check for mono/stereo and convert
#echo $(($(date +%s%N) / 100000000))
filename=$1
filetype=$(sox --i -t "$filename" | awk -F: 'File type/{print $2}' | tr -d '[space:]')
if [[ "$filetype" == "WAV" || "$filetype" == "AIFF" ]]; then

    if(soxi "/home/pi/_samples/$1" | grep Channels | grep 1 > /dev/null)
    then
        echo "this file is mono"
        sox -G "/home/pi/_samples/$1" -t raw -r 49148 -b 32 -c 1 -e floating-point "/home/pi/_converted/$1.raw" trim 0 13 &
        #cp /root/samples/load/Left /root/samples/load/Right
	wait
    else
        echo "this file is stereo"
        sox -G "/home/pi/_samples/$1" -t raw -r 49148 -b 32 -c 1 -e floating-point "/home/pi/_converted/$1_lch.raw" remix 1 trim 0 13 &
        sox -G "/home/pi/_samples/$1" -t raw -r 49148 -b 32 -c 1 -e floating-point "/home/pi/_converted/$1_rch.raw" remix 2 trim 0 13 &
	wait
    fi
    sync
else
	echo"ignore file, not converting this file to Raw"
fi
#echo $(($(date +%s%N) / 100000000))
#END=$(date +%s)
#DIFF=$(( $END - $START ))
#echo "It took $DIFF seconds"
