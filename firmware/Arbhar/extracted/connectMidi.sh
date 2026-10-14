#!/bin/sh

#while true;do

	FILE=/dev/snd/midiC1D0
	#FILE=/boot/calibrated
	#DIR=/dev/snd/
	#SEARCH=midiC1D0
	
	#grep $SEARCH $DIR*
#	ls /dev/snd/midiC1D0
	echo "$FILE"
	#aconnect 20:0 128:0
	if [ -c "$FILE" ]; then 
		echo "seeingMidiDev"
	else
		echo "noMidiDev"
	fi
#	sleep 3
#done
