# loadArbharV2
# setting up and loading arbhar v2

# THIS SHOULD BE AUTOMATICALLY COPIED TO /home/pi

echo "------------------"
echo "------------------"
echo "------------------"
echo "------------------"
echo "STARTING UP ARBHAR"
echo "------------------"
ls /home/pi/arbhar_v2/arbhar_v*
echo "------------------"
echo "------------------"
echo "------------------"

echo "setting scaling_governor to performance"
sudo bash arbhar_v2/setCpuSpeed.sh
#echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
#cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "------------------"

gpio -g write 41 1
sudo bash /home/pi/arbhar_v2/checkForImportingInternalScenesFromUSB.sh
gpio -g write 41 0

CHECK_DISPLAY=/boot/show-display
if [ -f "$CHECK_DISPLAY" ]; then
	#tvservice -p 
	#curr_vt='fgconsole'
	sudo vcgencmd display_power 1
	echo "display on"
else
	#tvservice -o
	sudo vcgencmd display_power 0
	echo "display off"
fi

echo "------------------"
#force alsa setup to ensure all channels are working
sudo alsactl --file /home/pi/alsa-setup-V1-7.state restore
amixer -c 0 contents
echo "------------------"
echo "check for calbration file"
bash /home/pi/arbhar_v2/checkForCalibratedFile.sh

#mount USB to check for additional scripts and preset.txt
echo "check USB for additional scripts and user preset.txt"
sudo sh /home/pi/arbhar_v2/mountusb.sh
sudo bash /media/usb/additionalScripts.sh
sudo bash /home/pi/arbhar_v2/checkForPresetEditor.sh
sudo bash /home/pi/arbhar_v2/checkForUserPreset.sh


bash /home/pi/arbhar_v2/checkButtonsOnStartup.sh

echo "check usb setup"
bash /home/pi/arbhar_v2/makeDirectorySystem.sh

sync
sleep 1

SOURCE_DIR="/home/pi/arbhar_v2/"

FILE=/boot/calibrated
if [ -f "$FILE" ]; then
	echo "checking for existing calibration data"
	sudo pd -nogui /home/pi/arbhar_v2/arbharCalibrationCheck.pd &
	echo "waiting to quit calibration check"
	wait
	echo "loading arbhar file system manager to load into shared memory"
	sudo taskset -c 3 sudo nice -n 20 sudo pd -noaudio -nogui "$SOURCE_DIR"loadFileToShMem2.pd &
	sleep 0.5
	echo "loadinging arbhar main audio engine"
	sudo nice -n -20 sudo pd -nogui -alsamidi -mididev 1,2,3 -alsa -r 48000 -audiobuf 8 "$SOURCE_DIR"arbharMain.pd &
	sleep 1
	echo  "loading arbhar controls and UI"
	sudo taskset -c 3 sudo nice -n -20 sudo pd -noaudio -nogui -r 48000 -audiobuf 25 "$SOURCE_DIR"arbharControls.pd &
	sleep 1
	echo "loading arbhar background processor"
	sudo sudo nice -n 20 sudo pd -noaudio -nogui "$SOURCE_DIR"awaitingProcessing.pd &

	echo "$FILE EXISTS, ARBHAR IS CALIBRATED."
else
	sudo nice -n -20 pd -alsa -r 48000 -audiobuf 25 -nogui "$SOURCE_DIR"arbharControls.pd &

	echo "CALIBRATION REQUIRED."
fi

sudo sh /home/pi/arbhar_v2/unmountusb.sh
