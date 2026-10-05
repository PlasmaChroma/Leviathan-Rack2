sudo bash /home/pi/arbhar_v2/mountusb.sh
number=0
for file in /media/usb/_updater/*; do
	if [[ -f "$file" ]]; then
		fileNoPath="${file##*-}"
		filename="${fileNoPath%.gz}"
		numberRaw=${filename%%[^0-9]}
		let "number = 10#$numberRaw"
		#echo "updater file $file, version number: $number"
#		echo $number
	fi
done
echo $number

#pattern="arbhar_updater_beta-"
#
#versions=$(ls /media/usb/_updater/ | grep "${pattern}" | sed "s/{$pattern}//" | cut -d '.' -f1)
#
#if [ -z "$versions" ]; then
#	echo "no files"
#	exit 1
#fi
#
#highest_version=$(echo "$versions" | tr ' ' '\n' | sort -n -r | awk '{ printf "%03d\n", $0 }' | head -n 1)

#echo $versions
#echo $highest_version
