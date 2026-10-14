#!/bin/bash

file_path="/home/pi/arbhar_log.txt"
destination="home/pi/arbhar_log_trunc.txt"
n=1000

line_count=$(wc -l < "$file_path")
echo "to start with arbhar_log.txt has "$line_count" lines";

if [[ $line_count -gt $n ]]; then
	#set -i "1,${lines_to_remove}d" "$file_path"
	#lines_to_remove=$((line_count - n))
	#echo "$lines_to_remove"
	tail -n $n "$file_path" > "$destination"
	#mv "$file_path.tmp" "$file_path"
fi


line_count=$(wc -l "$file_path")
echo "now arbhar_log.txt has "$line_count" lines";
