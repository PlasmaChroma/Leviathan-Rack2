sudo bash /home/pi/arbhar_v2/mountusb.sh

if [ ! -f /media/usb/preset.txt ]; then
	echo "no preset file on USB, looking for previous preset"

	if [ ! -f /home/pi/_autosave/preset.txt ]; then
		echo "no preset file in _autosave folder, copying from init file"
		sudo cp /home/pi/arbhar_v2/configurationDataInitFile.txt /home/pi/_autosave/preset.txt
	else
		echo "preset file exists in _autosave that will be used here"
	fi
else
	echo "copying preset file from USB stick to _autosave to be used"
	sudo cp /media/usb/preset.txt /home/pi/_autosave/
	sudo mv /media/usb/preset.txt /media/usb/preset_wasLoaded.txt
	echo "renamed the preset file on the usb stick to show it has been copied"
	#this will trigger loading the preset file in puredata.
	echo "successfully_copied_preset_file_to_load_immediately" 
fi
sleep 0.1
sync
echo "sync preset files to sd and usb"

sudo bash /home/pi/arbhar_v2/unmountusb.sh
