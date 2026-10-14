echo "EXECUTING FACTORY RESET SCRIPT"

sudo rm /home/pi/_autosave/*
sudo sox -r 49148 -b 32 -c 1 -n  /home/pi/_autosave/silence.raw trim 0.0 13.0 &
sudo cp /home/pi/arbhar_v2/configurationDataFile.txt /home/pi/_autosave/preset.txt &
wait
echo "copying the silence for new autosave failes"
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/0l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/0r_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/1l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/1r_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/2l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/2r_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/3l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/3r_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/4l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/4r_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/5l_auto.raw &
sudo cp /home/pi/_autosave/silence.raw /home/pi/_autosave/5r_auto.raw &
wait
#echo "clearing the local scenes"
#sudo rm /home/pi/_scenes/1_scene/* &
#sudo rm /home/pi/_scenes/2_scene/* &
#sudo rm /home/pi/_scenes/3_scene/* &
#sudo rm /home/pi/_scenes/4_scene/* &
#sudo rm /home/pi/_scenes/5_scene/* &
#sudo rm /home/pi/_scenes/6_scene/* &
echo "resetting local scene presets"
bash /home/pi/arbhar_v2/copyFactoryPresetFilesToInternalScenes.sh &

wait
echo "sync"
sync
echo "automatic reboot"
sudo reboot
