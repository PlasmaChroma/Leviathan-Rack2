sudo sh arbhar_v2/mountusb.sh

if ( grep "/media/usb" /proc/mounts > /dev/null )
then
	echo 42
else
	echo 6
fi

sudo sh arbhar_v2/unmountusb.sh
