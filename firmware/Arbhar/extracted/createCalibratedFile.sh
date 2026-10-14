echo "removing file '/boot/calibrate' that would force clibration"
if [ -f /boot/calibrate ]; then
        sudo rm /boot/calibrate
	echo "'calibrate' existed and is removed"
else
        echo "'calibrate' did not exist!"
fi


echo "creating calibrated file in home folder and on boot"
sudo touch /home/pi/calibrated
echo "calibrated" | sudo cat >> /home/pi/calibrated
sudo cp /home/pi/calibrated /boot
echo "checking file has been created"
if [ -f /home/pi/calibrated ]; then
	echo "'calibrated' exists"
	cat /home/pi/calibrated
else
	echo "'calibrated' file does not exist!"
fi

sync
echo "sync 'calibrated'file to /home/pi/ and /boot"
