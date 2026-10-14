#!/bin/bash

sudo sh /home/pi/arbhar_v2/mountusb.sh

directory="/media/usb/_arbhar_internal_scenes"
script="/home/pi/arbhar_v2/importInternalSceneFromUSB.sh"

if [ -d "$directory" ]; then
	echo "Directory $directory exists. copying directory to /home/pi/ ..."
	sudo cp -r $directory /home/pi/ &
	wait
	echo "_arbhar_internal_scenes copied to /home/pi/ "
	bash $script 1 &
	wait
	bash $script 2 &
	wait
	bash $script 3 &
	wait
	bash $script 4 &
	wait
	bash $script 5 &
	wait
	bash $script 6 &
	wait
	sync
	echo "removing _arbhar_internal_scenes from /home/pi/ "
	sudo rm -r /home/pi/_arbhar_internal_scenes
else
	echo "Directory $directory doesn't exist"
fi

sudo sh /home/pi/arbhar_v2/unmountusb.sh
