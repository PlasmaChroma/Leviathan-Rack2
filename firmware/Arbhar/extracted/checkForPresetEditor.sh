sudo bash /home/pi/arbhar_v2/mountusb.sh

#if [ ! -f /media/usb/arbhar_Preset_Editor.html ]; then
#	echo "no Preset Editor on USB, copying Editor from base files"

	sudo cp /home/pi/arbhar_v2/arbhar_Preset_Editor.html /media/usb/

#else
#	echo "Preset Editor exists on USB stick"
#fi

sync

sudo bash /home/pi/arbhar_v2/unmountusb.sh
