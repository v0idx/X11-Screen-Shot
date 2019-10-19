#!/bin/bash
#Get random filename
fn=$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 32)
image='/home/v0idx/Pictures/imageStore/'"$fn"'.png'

#Execute screenshot functionality
./main "$image"

#Copy to clipboard
xclip -selection clipboard -t image/png -i "$image"
