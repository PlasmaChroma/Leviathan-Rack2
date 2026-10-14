SCENE=$1

DEST=/home/pi/_autosave/
SOURCE=/home/pi/_scenes/
PRESET="$SOURCE$SCENE"_scene/preset.txt

in_file="$SOURCE$SCENE"_scene/preset.txt
out_file="$DEST"preset.txt

number=$(grep -oP "LoadConfiguration:\s+\K\d+" "$PRESET")
        echo "LoadConfiguration = "$number""

old_line="PARAMETER: LoadConfiguration: "$number""
new_line="PARAMETER: LoadConfiguration: 3"


if [ "$number" -ge 2 ]; then
	sudo nice -n -7 cp "$SOURCE$SCENE"_scene/*.raw "$DEST" &
fi
#sudo nice -n -7 cp "$SOURCE$SCENE"_scene/preset.txt "$DEST" &
sudo cp $in_file $out_file
#sudo awk -v new_line="$new_line" '/^PARAMETER: LoadConfiguration:/ {$0=new_line} 1' "$in_file" > "${out_file}.tmp" && sudo mv "${out_file}.tmp" "$out_file" &
sudo sed -i "s/$old_line/$new_line/g" "$out_file"
wait
echo "sync from local scene loader"
sync
echo "finishedLoadingSceneFromLocal"
