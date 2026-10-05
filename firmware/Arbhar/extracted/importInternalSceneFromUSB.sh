#sudo sh /home/pi/arbhar_v2/mountusb.sh

if ( grep "/media/usb" /proc/mounts > /dev/null )
then
	first_iteration=true
	SCENE="$1"_scene/
	PRESET_DIR=/home/pi/_arbhar_internal_scenes/"$SCENE"
	PRESET=/home/pi/_scenes/"$SCENE"preset.txt
#	INITIAL=/media/usb/loadPresetOnStartup.txt

#	bash /home/pi/arbhar_v2/findPresetFiles.sh "$PRESET_DIR"
	sudo cp "$PRESET_DIR"preset.txt $PRESET
#	number=$(grep -oP "LoadConfiguration:\s+\K\d+" "$PRESET")
#	echo "LoadConfiguration = "$number""
#	if [ "$number" -ge 2 ]; then
		OLDIFS=$IFS
		IFS=$'\n'
		#echo "loading the scene audio files:"
		FILES=$(find /media/usb/_arbhar_internal_scenes/$SCENE -type f \( -name "*.wav" -o -name "*.aif" \) -not -name '*.txt'  -not -name ".*")
	
		SORTED_FILES=($(printf '%s\n' "${FILES[@]}" | sort))
		noOfFiles=0;
		echo "startLoadingScene"
		for i in "${SORTED_FILES[@]}"
		do
			if [[ $noOfFiles -le 5 ]]
				then
				echo "$i"
				if(soxi "$i" | grep Channels | grep 1 > /dev/null)
				then
					echo "mono"
					sudo nice -n -20 sudo sox "$i" -q -G -r 49148 -b 32 -e floating-point -c 1 /home/pi/_scenes/"$SCENE""$noOfFiles"l_auto.raw remix 1 trim 0 13 &
					sudo nice -n -20 sudo sox "$i" -q -G -r 49148 -b 32 -e floating-point -c 1 /home/pi/_scenes/"$SCENE""$noOfFiles"r_auto.raw remix 1 trim 0 13 &
					wait
					sync
				else
					echo "stereo"
					sudo nice -n -20 sudo sox "$i" -q -G -r 49148 -b 32 -e floating-point -c 1 /home/pi/_scenes/"$SCENE""$noOfFiles"l_auto.raw remix 1 trim 0 13 &
					sudo nice -n -20 sudo sox "$i" -q -G -r 49148 -b 32 -e floating-point -c 1 /home/pi/_scenes/"$SCENE""$noOfFiles"r_auto.raw remix 2 trim 0 13 &
					wait
					sync
				fi
				echo "$noOfFiles"
				let noOfFiles++
			fi
		done
		IFS=$OLDIFS
		wait
		#if [ "$number" -eq 3 ]; then
		#	echo "load preset file: "$PRESET""
		#	sudo cp $PRESET /home/pi/_autosave/preset.txt
		#	sync
		#else
		#	echo "preset file is ignored"
		#fi
	#else
	#	echo "no audiofile will be loaded"
	#	if [ "$number" -eq 1 ]; then
	#		echo "load preset file: "$PRESET""
	#		sudo cp $PRESET /home/pi/_autosave/preset.txt
	#		sync
	#	else
	#		echo "preset file is ignored"
	#	fi 
	#fi

	echo "sync from copying internal scene from USB"
	sync
	#echo "completedLoadingSceneFromUSB"
else
	echo "noUSB!!"
fi

#sudo sh /home/pi/arbhar_v2/unmountusb.sh

exit 0
