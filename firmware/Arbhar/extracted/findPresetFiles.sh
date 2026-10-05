#!/bin/bash

#bash /home/pi/arbhar_v2/mountusb.sh


directory=$1
#if ! grep -qrl '^#ARB' "$directory"; then
#	sudo cp /home/pi/arbhar_v2/configurationDataFile.txt "$directory"
#fi

mapfile -d '' -t files < <(find "$directory" -type f -name "*.txt" -exec grep -l "^#ARB" {} +)
file_array=($(printf "%s\0" "${files[@]}" | sort -z | xargs -0))

#files=()

#while IFS= read -r -d '' file; do
#	$files+=("$file")
#done < <(find "$directory" -type f -name "*.txt" -exec grep -l "^#ARB" {} +)

number=0

first_iteration=true

for filename in "${file_array[@]}"; do
#for filename in "${files[@]}"; do
	echo "preset file: $filename"
	#line=$(grep "^\s*PRESET_NAME:" "$filename")
	#echo "$line"
	if [ "$first_iteration" = true ]; then
		echo "$filename"
		grep -e "^\s*PRESET_NAME:" -e "^\s*PARAMETER:" "$filename"
		number=$(grep -oP "LoadConfiguration:\s+\K\d+" "$filename")
        	echo "LoadConfiguration = "$number""
		if [ $number -eq 2 ]; then
			echo "preset file will not be copied into _autosave"
		else
				echo "copy preset file into _autosave"
			sudo cp $filename /home/pi/_autosave/preset.txt
			sync
			if [ $? -eq 0 ]; then
				echo "sucessfully copied preset file, here is it's content:"
				grep -e "^\s*PRESET_NAME:" -e "^\s*PARAMETER:" "/home/pi/_autosave/preset.txt"
			else
				echo "file copy failed"
			fi
		fi
		sync
		first_iteration=false
	fi
done
echo "return LoadConfiguration settting: $number"
return $number
