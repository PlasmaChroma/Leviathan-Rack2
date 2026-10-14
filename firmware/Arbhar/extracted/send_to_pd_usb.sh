#!/bin/bash

ACTION=$2
#LOGFILE="/home/pi/udev_test.log"
LOGFILE="tmp/pd_pipe"

if [[ ! -p /tmp/pd_pipe ]]; then
	mkfifo /tmp/pd_pipe
fi

#echo "$(date): Action=$ACTION, Device=$1" >> $LOGFILE

if [ "$ACTION" = "add" ]; then
    echo "adding $1" >> $LOGFILE
elif [ "$ACTION" = "remove" ]; then
    echo "removing $1" >> $LOGFILE
fi
