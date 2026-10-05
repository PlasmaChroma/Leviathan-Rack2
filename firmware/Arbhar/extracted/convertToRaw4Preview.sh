#MAKE PREVIEW FILE

sudo sh /home/pi/arbhar_v2/mountusb.sh

if [ ! -d "/mnt/ramdisk/_temp" ]; then
	sudo mkdir /mnt/ramdisk/_temp
	exho "_temp directory created"
else
	echo "_temp exists"
fi
 
#sudo rm /mnt/ramdisk/_temp/*

#ls /media/usb

echo "going to process folder $1"

sudo rm /mnt/ramdisk/_temp/*
sudo mkdir /mnt/ramdisk/_temp

SOURCE_DIR="$1"
ls -l -a $SOURCE_DIR
#files=(
#	"$SOURCE_DIR"/*.aif
#	"$SOURCE_DIR"/*.wav
#)
#files=$(find "$SOURCE_DIR"/ -type f \( -name "*.wav" -o -name "*.aif" \) -not -path "*/\.*")
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
#			echo "file: "$file""
		fi
	else
		thereAreFiles=0
	fi
done

echo "we have some files? "$thereAreFiles""

if [ "$thereAreFiles" -eq 1 ]; then

	for i in "${files[@]}"
	do
	if $first_iteration; then
		echo "file to process: "${i##*/}""
		echo "name of i: $i"
		f="${i##*/}"
		fileName="${f// /_}"
	#	nullName=$(find . -name "$i" -print0)
	#	while IFS= read -r -d '' ffile; do
	#	       	echo "$ffile"
	#	done <<< "$nullName"
		#soxi -c $i
	#
		OLDIFS=$IFS
	
		IFS='\n'

		#nullName="${i// /$'\0'}"
		nullifyName=$(printf "%s" "$f" | tr ' ' '\0')
		nullName="${SOURCE_DIR}/${fileName}"
		sudo mv "$i" "$nullName"

		IFS=$OLDIFS
		echo "name with null: $nullName"
		CHANNELS=$(soxi -c "$nullName")
		first_iteration=true
		echo "channels: "$CHANNELS""
		if [[ "$CHANNELS" == *"1"* ]]
        	then
                	echo ""$nullName" is mono ("$CHANNELS")"
			if $first_iteration; then
				sudo nice -n -10 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3
				first_iteration=false
			fi
#               	sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/${fileName%%.*}_M_L.raw trim 0 13 &
			#sox -q -G "$i" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/${fileName%%.*}_R.raw trim 0 13
			#cp /mnt/ramdisk/_temp/${fileName%%.*}_M.raw  /mnt/ramdisk/_temp/${fileName%%.*}_L.raw
#			wait
#			cp /mnt/ramdisk/_temp/${fileName%%.*}_M_L.raw  /mnt/ramdisk/_temp/${fileName%%.*}_M_R.raw
		elif [[ "$CHANNELS" == *"2"* ]]
        	then
                	echo  ""$nullName" is stereo ("$CHANNELS")"
			if $first_iteration; then
				sudo nice -n -10 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3 
				first_iteration=false
			fi
#               	 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_L.raw remix 1 trim 0 13 &
#               	sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_R.raw remix 2 trim 0 13 &
#			wait
	       else
			echo  ""$nullName" is at least stereo ("$CHANNELS")"
                        if $first_iteration; then
                                sudo nice -n -10 sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point  /mnt/ramdisk/_temp/preview.raw trim 0 3 
                                first_iteration=false
                        fi
#                        sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_L.raw remix 1 trim 0 13 &
#                       sox -q -G "$nullName" -t raw -r 49148 -b 32 -c 1 -e floating-point /mnt/ramdisk/_temp/${fileName%%.*}_S_R.raw remix 2 trim 0 13 &
#                       wait

	                echo "something might go wrong"
	        fi
#
	fi
	done
	sync
#	echo "all_converted"
	echo "preview_file_converted"
else
	echo "nothing_to_convert"
fi

sudo sh /home/pi/arbhar_v2/unmountusb.sh
