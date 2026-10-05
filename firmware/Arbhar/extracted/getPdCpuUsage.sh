while true; do
	echo "***************"
	pids=$(pgrep pd)
	for pid in $pids; do
		if ps -p $pid > /dev/null; then
			#echo "$pid pd"
			cpu_usage=$(top -b -n 1 -p $pid | grep -w $pid | awk '{print $9}')
			if (( $(awk 'BEGIN {print ('$cpu_usage' > 0)}') )); then
				#cmdline=$(tr '\0' ' ' < /proc/$PID/cmdline)
				#echo "$pid $cpu_usage : $cmdline"
				echo "$pid : $cpu_usage"
			fi
		fi
	done
	sleep 5
done
