#MAKE PREVIEW FILE

sudo sh /home/pi/arbhar_v2/mountusb.sh

if [ ! -d "/mnt/ramdisk/_temp" ]; then
	sudo mkdir /mnt/ramdisk/_temp
	exho "_temp directory created"
else
	echo "_temp exists"
fi
 
sudo rm /mnt/ramdisk/_temp/*

echo "going to process folder $1"

sudo rm /mnt/ramdisk/_temp/*
sudo mkdir /mnt/ramdisk/_temp

SOURCE_DIR="$1"
files=()
thereAreFiles=1
for file in "$SOURCE_DIR"/*; do
	if [[ -f "$file" ]]; then
		thereAreFiles=1
		#filetype=$(sox --i -t "$file" | awk -F '/File Type/{print $2}' | tr -d '[:space:]')
		filetype=$(file -b --mime-type "$file")
#		echo "$filetype"
		if [[ "$filetype" == "audio/wav" || "$filetype" == "audio/aiff" || "$filetype" == "audio/x-wav" || "$filetype" == "audio/x-aiff" ]]; then
		#if [[ "$filetype" == "WAV" || "$filetype" == "AIFF" ]]; then
			files+=("$file")
#			echo "$file"
		fi
	else
		thereAreFiles=0
	fi
done

echo "we have some files? "$thereAreFiles""

if [ "$thereAreFiles" -eq 1 ]; then

	for i in "${files[@]}"
	do
		echo "file to process: "${i##*/}""
		echo "name of i: $i"
		f="${i##*/}"
		fileName="${f// /_}"
	OLDIFS=$IFS

	IFS='\n'

	nullifyName=$(printf "%s" "$f" | tr ' ' '\0')
	nullName="${SOURCE_DIR}/${fileName}"
	sudo mv "$i" "$nullName"

	IFS=$OLDIFS
	echo "name with null: $nullName"
		CHANNELS=$(soxi -c "$nullName")
		first_iteration=true
		echo "channels:"$CHANNELS""
		if [[ "$CHANNELS" == *"1"* ]]
        	then
                	echo ""$nullName" is mono ("$CHANNELS")"
			if $first_iteration; then
#				sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3 &
				first_iteration=false
			fi
	               	sudo nice -n 7 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/${fileName%%.*}_M_L.raw trim 0 13 &
			wait
			cp /mnt/ramdisk/_temp/${fileName%%.*}_M_L.raw  /mnt/ramdisk/_temp/${fileName%%.*}_M_R.raw
		elif [[ "$CHANNELS" == *"2"* ]]
        	then
                	echo  ""$nullName" is used as stereo ("$CHANNELS")"
			if $first_iteration; then
#				sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3 &
				first_iteration=false
			fi
	           	sudo nice -n 7 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_L.raw remix 1 trim 0 13 &
	               	sudo nice -n 7 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_R.raw remix 2 trim 0 13 &
			wait
	       else
			echo  ""$nullName" is at least stereo ("$CHANNELS")"
                        if $first_iteration; then
#                               sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3 &
                                first_iteration=false
                        fi
                        sudo nice -n 7 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_L.raw remix 1 trim 0 13 &
                        sudo nice -n 7 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_R.raw remix 2 trim 0 13 &
                        wait

	                echo "something might be wrong"
	        fi
#

	done
	sync
	echo "all_converted"

else
	echo "nothing_to_convert"
fi

sudo sh /home/pi/arbhar_v2/unmountusb.sh
