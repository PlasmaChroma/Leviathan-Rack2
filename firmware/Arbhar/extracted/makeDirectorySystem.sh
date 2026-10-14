#make library stucture

echo "mount usb"
sudo sh /home/pi/arbhar_v2/mountusb.sh

if ( grep "/media/usb" /proc/mounts > /dev/null )
then

sleep 1

DIR="/media/usb/_arbhar_library"
if [ ! -d $DIR ]; then
	sudo mkdir $DIR
	echo "created "$DIR""
	for lbank in {1..6}
	do
		for lslot in {1..6}
		do
			SLOT_DIR=$DIR/"$lbank"_"$lslot"_sample
			#echo "?? "$SLOT_DIR""
			if [ ! -d $SLOT_DIR ]; then
				sudo mkdir $SLOT_DIR
				echo "created the directory "$SLOT_DIR""
			else
				echo "directory "$SLOT_DIR" exists already"
			fi
		done
	done
	echo "sync from directory maker"
	sync
else
	echo ""$DIR" exists aleady"
fi

echo "finished _library structure"

#make scene structure

DIR="/media/usb/_arbhar_scenes"

if [ ! -d $DIR ]; then
	sudo mkdir $DIR
	echo "created "$DIR""
	for bank in {1..6}
	do
		for slot in {1..6}
		do
			SLOT_DIR=$DIR/"$bank"_"$slot"_scene
			echo "?? "$SLOT_DIR""
			if [ ! -d $SLOT_DIR ]; then
				sudo mkdir $SLOT_DIR
				echo "created the directory "$SLOT_DIR""
				sudo cp /home/pi/arbhar_v2/configurationDataInitFile.txt $SLOT_DIR/preset.txt
				echo "copied default preset into scene slot"
			else
				sudo echo "directory "$SLOT_DIR" exists already"
				if [ -f $SLOT_DIR/preset.txt ]; then
					echo "preset file exists already"
				else
					sudo cp /home/pi/arbhar_v2/configurationDataInitFile.txt $SLOT_DIR/preset.txt
					echo "copy preset file into "$SLOT_DIR""
				fi
			fi
		done
	done

else
	echo ""$DIR" exists aleady"
fi

echo "finished _scenes stucture"

echo "check for _updater"
if [ ! -d /media/usb/_updater ]; then
	echo "create _updater"
	sudo mkdir /media/usb/_updater
else
	echo "_updater exists"
fi

#echo "changing permissions"

#sudo chown -R pi:pi /media/usb/_arbhar_library
#sudo chgrp -R "pi" /media/usb/_arbhar_library
#sudo chown -R pi:pi /media/usb/_arbhar_scenes
#sudo chgrp -R "pi" /media/usb/_arbhar_scenes

echo "unmount usb"
sudo sh /home/pi/arbhar_v2/unmountusb.sh

else
	echo "no usb inserted"
fi
