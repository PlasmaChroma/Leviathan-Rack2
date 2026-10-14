sudo bash /home/pi/arbhar_v2/mountusb.sh

echo "copying log file to usb"
sudo cp /home/pi/arbhar_log.txt /media/usb
sync
echo "log file has been copied"

sudo bash /home/pi/arbhar_v2/unmountusb.sh

