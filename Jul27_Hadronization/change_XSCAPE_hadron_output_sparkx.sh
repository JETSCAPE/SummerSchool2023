#!/bin/bash

# Specify the directory path
directory="../../X-SCAPE/build/hadronization_results/"

# Change to the specified directory
cd "$directory"

# Loop through all .dat files in the directory
for file in *.dat; do
  # Check if the file exists and is a regular file
  if [ -f "$file" ]; then
    # Get the last line of the file
    last_line=$(tail -n 1 "$file")

    # Remove the "weight value pT-Hat value" part from the last line
    modified_line=$(echo "$last_line" | sed -E 's/weight\s+[0-9.]+\s+pT-Hat\s+[0-9.]+//')

    # Replace the last line with the modified line
    sed -i '$s/.*/'"$modified_line"'/' "$file"

    echo "Modified $file"
  fi
done
