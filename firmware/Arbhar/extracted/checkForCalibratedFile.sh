echo "check for 'calibrated'"
if [ -f /home/pi/calibrated ]; then
	sudo cp /home/pi/calibrated /boot
	echo "'calibrated' copied to /boot"
else
	sudo rm /boot/calibrated
fi

if [ -f /boot/calibrate ]; then
	sudo rm /boot/calibrated
	sudo rm /home/pi/calibrated
fi

sync
echo "sync files to sd"
