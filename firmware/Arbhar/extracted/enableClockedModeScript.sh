#!/bin/bash

# The directory to start from
TARGET_DIR=$1

# Define the old and new text patterns
OLD_LINE="PARAMETER: EnableClockedModeSwitch: 0"
NEW_LINE="PARAMETER: EnableClockedModeSwitch: 1"
INSERT_MARKER="PARAMETER: QuantiseTable:"
INSERT_CONTENT_FILE="./clockedModePresetTextAddition.txt"

# Find all text files in the directory and its subdirectories
# -name "*.txt" restricts the search to text files
find "$TARGET_DIR" -type f -name "*.txt" | while read -r FILE
do
  # Check if the file contains #ARB
  if grep -qF "#ARB" "$FILE"; then
    # Check if the file contains the old line to be replaced
    if grep -qF "$OLD_LINE" "$FILE"; then
      echo "Updating $FILE (replacing existing line)"
      # Replace the line in-place with sed
      sed -i "s/$OLD_LINE/$NEW_LINE/g" "$FILE"
    else
      echo "Skipping line replacement in $FILE (old line not found)"
    fi

    # Check if the file does not contain the target parameter for insertion
    if ! grep -qF "PARAMETER: EnableClockedModeSwitch:" "$FILE"; then
      echo "Inserting content into $FILE"
      # Create a temporary file to hold the new content
      TEMP_FILE="temp_$RANDOM.txt"
      
      # Insert content after PARAMETER: QuantiseTable:
      awk -v insert_content="$(cat $INSERT_CONTENT_FILE)" \
          -v insert_after="$INSERT_MARKER" \
          '
          BEGIN { found_insert_point = 0 }
          {
            print
            if (found_insert_point == 0 && $0 ~ insert_after) {
              print insert_content
              found_insert_point = 1
            }
          }
          ' "$FILE" > "$TEMP_FILE"
      
      # Replace the original file with the temporary file
      mv "$TEMP_FILE" "$FILE"
    else
      echo "Skipping content insertion in $FILE (target parameter already exists)"
    fi
  else
    echo "Skipping $FILE (does not contain #ARB)"
  fi
done

echo "Script execution complete."
