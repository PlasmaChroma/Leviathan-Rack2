echo "33" > /sys/class/gpio/export
echo "42" > /sys/class/gpio/export
echo "38" > /sys/class/gpio/export

shiftButton=$( cat /sys/class/gpio/gpio33/value )
captureButton=$( cat /sys/class/gpio/gpio42/value )
strikeButton=$( cat /sys/class/gpio/gpio38/value )
#part=$(($shiftButton + $captueButton))
sum=$((!shiftButton + !captureButton + !strikeButton))
echo "Sum of all buttons currently pressed: $sum"
if [[ $sum -eq 3 ]]
then
	echo "deleting 'calibrated' file"
	sudo rm /boot/calibrated
	echo "sync after deleting 'calibrated' file"
	sync
else
	echo "keeping previous calibration data"
fi
echo "33" > /sys/class/gpio/unexport
echo "42" > /sys/class/gpio/unexport
echo "38" > /sys/class/gpio/unexport
