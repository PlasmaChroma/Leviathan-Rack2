grep '\b.\+:' /home/pi/calibrationDataFile.txt > /home/pi/getCalibrationValues.txt

sleep 0.2
sync
echo "done"
