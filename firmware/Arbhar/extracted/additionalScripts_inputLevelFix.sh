#!/bin/bash

file="/home/pi/alsa-setup-V1-7.state"
# for arbhar harware version = 1.5 set the following
# new_value_0 to 31 and
# new_value_1 to 23
# for arbhar harware version > 1.7 set both values to 23
new_value_0=31  # Input Gain for Input (channel 1)
new_value_1=23  # Input Gain for Onset (channel 2)

sudo awk -v new_value_0="$new_value_0" -v new_value_1="$new_value_1" '
{
    if (found) {
        if (++count == 1) {
            sub(/[0-9]+$/, new_value_0);
        } else if (count == 2) {
            sub(/[0-9]+$/, new_value_1);
            found = 0;
        }
        print;
    }
    else {
        print;
    }
}
/name '\''Capture Volume'\''/ { found = 1; count = 0; }
' "$file" > tmpfile && mv tmpfile "$file"

# Check the exit status of the mv command
if [ $? -eq 0 ]; then
    echo "Process was successful."
    sudo touch /media/usb/PatchForArbharHardware1-5_Successful
    sudo touch /boot/PatchForArbharHardware1-5_Successful
else
    echo "Process failed."
    sudo touch /media/usb/PatchForArbharHardware1-5_Failed
    sudo touch /boot/PatchForArbharHardware1-5_Failed
fi
