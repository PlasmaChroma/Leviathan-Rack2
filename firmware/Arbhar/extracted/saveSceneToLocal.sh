SCENE=$1
SOURCE=/home/pi/_autosave/
DEST=/home/pi/_scenes/

sudo nice -n -7 cp  "$SOURCE"* "$DEST$SCENE"_scene &
sudo nice -n -7 cp /home/pi/_autosave/preset.txt "$DEST$SCENE"_scene/preset.txt &
wait
sync
echo "sync from local scene saver"
echo "finishedSavingSceneToLocal"
