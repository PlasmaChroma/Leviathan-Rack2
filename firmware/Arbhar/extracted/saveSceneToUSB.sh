
sudo bash /home/pi/arbhar_v2/makeDirectorySystem.sh &
wait

sudo sh /home/pi/arbhar_v2/mountusb.sh

SOURCE=/home/pi/_autosave
DESTINATION=/media/usb/_arbhar_scenes

if ( grep "/media/usb" /proc/mounts > /dev/null )
then
	SCENE="$1"_"$2"_scene
	sh /home/pi/arbhar_v2/renameFiles.sh $DESTINATION/$SCENE &
	wait
	sync
	echo "$SCENE"
	echo "startSavingScenePresetFile"
	sudo nice -n -12 cp /home/pi/_autosave/preset.txt $DESTINATION/$SCENE/preset.txt &
	wait
	sync
	echo "startSavingSceneToUSB"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/0l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/0r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/1_alpha.wav &
	wait
	sync
	echo "one is done"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/1l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/1r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/2_beta.wav &
	wait
	sync
	echo "two is done"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/2l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/2r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/3_gamma.wav &
	wait
	sync
	echo "three is done"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/3l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/3r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/4_delta.wav &
	wait
	sync
	echo "four is done"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/4l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/4r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/5_epsilon.wav &
	wait
	sync
	echo "five is done"
	sudo nice -n -12 sox -M -q -G -r 49148 -b 32 -e floating-point -c 1 $SOURCE/5l_auto.raw -q -G -r 49148 -c 1 -b 32 -e floating-point $SOURCE/5r_auto.raw -r 48000 -c 2  $DESTINATION/$SCENE/6_zeta.wav &
	wait
	echo "completedSavingSceneToUSB"
	sync
	echo "last sync from save scene to USB "
else
	echo "noUSB!!"
fi

sudo sh /home/pi/arbhar_v2/unmountusb.sh
