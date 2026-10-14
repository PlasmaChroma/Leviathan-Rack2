grep -w PARAMETER: _autosave/preset.txt > testFormatString.txt 
sleep 0.2
sync
#cat testFormatString.txt
echo "done"
grep -w PRESET_NAME: _autosave/preset.txt
