#!/bin/bash
#Get random filename
$Directory = "~/imageStore/"
fn=$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 32)
if [-d $Directory]; then 
  mkdir $Directory
fi
image="${Directory} ${fn}.png"

#Execute screenshot functionality
./main "$image"

#Copy to clipboard
xclip -selection clipboard -t image/png -i "$image"
