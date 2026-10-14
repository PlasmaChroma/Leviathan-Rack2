bash /home/pi/arbhar_v2/mountusb.sh

echo "loading the scene audio files:"
FILES=$(find /media/usb/_arbhar_scenes/1_1_scene/ -type f \( -name "*.wav" -o -name "*.aif" \))
SORTED_FILES=($(printf '%s\n' "${FILES[@]}" | sort -1 -g))
echo "startLoadingScene"
for i in "${SORTED_FILES[@]}"
do
	echo "$i"
done
