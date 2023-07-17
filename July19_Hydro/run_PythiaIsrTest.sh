#!/bin/bash

folder_path="String_files_sub_$1"

if [ -d "$folder_path" ]; then
  rm -r $folder_path
fi

mkdir $folder_path
for((i=0; i<1; i++)); do
    ./PythiaIsrTest
    mv strings_event_0.dat $folder_path/$i
done
