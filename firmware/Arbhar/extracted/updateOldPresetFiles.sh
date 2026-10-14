#!/bin/bash

# Define the function to process a folder
process_folder() {
    if [ -f "$1/preset.txt" ]; then
        if ! grep -q "#ARB" "$1/preset.txt"; then
            sed -i '1s/^/#ARB\n/' "$1/preset.txt"
            echo "Inserted #ARB in $1/preset.txt"
        else
            echo "#ARB already exists in $1/preset.txt"
        fi
    else
        echo "preset.txt not found in $1"
    fi
}

# Main script
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <root_folder>"
    exit 1
fi

root_folder="$1"

if [ ! -d "$root_folder" ]; then
    echo "$root_folder is not a valid directory"
    exit 1
fi

find "$root_folder" -type d | while read folder; do
    process_folder "$folder"
done

echo "Script completed."
