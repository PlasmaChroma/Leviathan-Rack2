sudo sh /home/pi/arbhar_v2/mountusb.sh


CHECK_FOR_FILE="/media/usb/cpumanaging.txt"

if [ -f "$CHECK_FOR_FILE" ]; then
	read -r cpu_managing < "/media/usb/cpumanaging.txt"
	echo "new cpu managing approach read from file: "$cpu_managing""
	echo $cpu_managing | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
else
	echo "no new cpu management approach found, ensuring it is set to performance"
	echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
fi


if [ ! -e "/media/usb/cpufreq.txt" ]; then
	echo "No instruction to change cpu speed available (no cpufreq.txt file on the USB storage). Exiting script."
	exit 1
fi

declare -i arm_freq=$(grep -E "^arm_freq=" /boot/config.txt | cut -d "=" -f2)
declare -i cpufreq=$(head -n1 /media/usb/cpufreq.txt)

echo "comparing cpu settings in config.txt and cpufreq.txt" 
if (( cpufreq >= 800 && cpufreq <= 1500 )); then
	if [ "$arm_freq" -ne "$cpufreq" ]; then
		read -r cpu_freq < "/media/usb/cpufreq.txt"
		echo "new cpu speed read from file: "$cpu_freq""
		sudo sed -i "s/^arm_freq=.*/arm_freq=$cpu_freq/" /boot/config.txt
		grep "^arm_freq=" /boot/config.txt | awk -F '=' '{print $2}'
		sync
		sudo reboot
	else
		echo "no new cpu speed found"
	fi
else
	echo "cpufreq not within range of 800 to 1500"
fi
