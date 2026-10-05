#!bin/bash
# This script copies user preset files from a USB drive to the internal scenes folder
# The user preset files are named 1_preset.txt, 2_preset.txt, 3_preset.txt, 4_preset.txt, 5_preset.txt, 6_preset.txt
# The script also renames the folder named _copy_user_presets_transferred to prevent the files from being copied again
# The script is intended to be run from the USB drive


sudo sh /home/pi/arbhar_v2/mountusb.sh

if [ -f /media/usb/_copy_user_presets/1_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/1_preset.txt /home/pi/_scenes/1_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/1_preset.txt /media/usb/_copy_user_presets/1_preset_wasCopiedToInternalSceneAlpha.txt
    echo "1_preset.txt copied to internal scene Alpha"
elif [ -f /media/usb/_copy_user_presets/alpha_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/alpha_preset.txt /home/pi/_scenes/1_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/alpha_preset.txt /media/usb/_copy_user_presets/alpha_preset_wasCopiedToInternalSceneAlpha.txt
    echo "alpha_preset.txt copied to internal scene Alpha"
else
    echo "No preset file 1_preset.txt nor alpha_preset.txt found"
fi

if [ -f /media/usb/_copy_user_presets/2_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/2_preset.txt /home/pi/_scenes/2_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/2_preset.txt /media/usb/_copy_user_presets/2_preset_wasCopiedToInternalSceneBeta.txt
    echo "2_preset.txt copied to internal scene Beta"
elif [ -f /media/usb/_copy_user_presets/beta_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/beta_preset.txt /home/pi/_scenes/2_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/beta_preset.txt /media/usb/_copy_user_presets/beta_preset_wasCopiedToInternalScenebBeta.txt
    echo "beta_preset.txt copied to internal scene Beta"
else
    echo "No preset file 2_preset.txt nor beta_preset.txt found"
fi

if [ -f /media/usb/_copy_user_presets/3_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/3_preset.txt /home/pi/_scenes/3_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/3_preset.txt /media/usb/_copy_user_presets/3_preset_wasCopiedToInternalSceneGamma.txt
    echo "3_preset.txt copied to internal scene Gamma"
elif [ -f /media/usb/_copy_user_presets/gamma_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/gamma_preset.txt /home/pi/_scenes/3_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/gamma_preset.txt /media/usb/_copy_user_presets/gamma_preset_wasCopiedToInternalSceneGamma.txt
    echo "gamma_preset.txt copied to internal scene Gamma"
else
    echo "No preset file 3_preset.txt nor gamma_preset.txt found"
fi

if [ -f /media/usb/_copy_user_presets/4_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/4_preset.txt /home/pi/_scenes/4_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/4_preset.txt /media/usb/_copy_user_presets/4_preset_wasCopiedToInternalSceneDelta.txt
    echo "4_preset.txt copied to internal scene Delta"
elif [ -f /media/usb/_copy_user_presets/delta_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/delta_preset.txt /home/pi/_scenes/4_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/delta_preset.txt /media/usb/_copy_user_presets/delta_preset_wasCopiedToInternalSceneDelta.txt
    echo "delta_preset.txt copied to internal scene Delta"
else
    echo "No preset file 4_preset.txt nor delta_preset.txt found"
fi

if [ -f /media/usb/_copy_user_presets/5_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/5_preset.txt /home/pi/_scenes/5_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/5_preset.txt /media/usb/_copy_user_presets/5_preset_wasCopiedToInternalSceneEpsilon.txt
    echo "5_preset.txt copied to internal scene Epsilon"
elif [ -f /media/usb/_copy_user_presets/epsilon_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/epsilon_preset.txt /home/pi/_scenes/5_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/epsilon_preset.txt /media/usb/_copy_user_presets/epsilon_preset_wasCopiedToInternalSceneEpsilon.txt
    echo "epsilon_preset.txt copied to internal scene Epsilon"
else
    echo "No preset file 5_preset.txt nor epsilon_preset.txt found"
fi

if [ -f /media/usb/_copy_user_presets/6_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/6_preset.txt /home/pi/_scenes/6_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/6_preset.txt /media/usb/_copy_user_presets/6_preset_wasCopiedToInternalSceneZeta.txt
    echo "6_preset.txt copied to internal scene Zeta"
elif [ -f /media/usb/_copy_user_presets/zeta_preset.txt ]; then
    sudo cp /media/usb/_copy_user_presets/zeta_preset.txt /home/pi/_scenes/6_scene/preset.txt
    sudo mv /media/usb/_copy_user_presets/zeta_preset.txt /media/usb/_copy_user_presets/zeta_preset_wasCopiedToInternalSceneZeta.txt
    echo "zeta_preset.txt copied to internal scene Zeta"
else
    echo "No preset file 6_preset.txt nor zeta_preset.txt found"
fi

sync

sudo sh /home/pi/arbhar_v2/unmountusb.sh
# sudo mv /media/usb/_copy_user_presets /media/usb/_copy_user_presets_transferred
