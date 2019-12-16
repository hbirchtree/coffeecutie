#!/bin/bash

ICON_DESTINATION="Coffee.iconset"
SRC_ICON="$1"

# echo "gen_icons.sh: Using icon ${SRC_ICON}"

mkdir -p ${ICON_DESTINATION}

iconsizes[0]=16
iconsizes[1]=32
iconsizes[2]=32
iconsizes[3]=64
iconsizes[4]=128
iconsizes[5]=256
iconsizes[6]=256
iconsizes[7]=512
iconsizes[8]=512
iconsizes[9]=1024

icon_file[0]="${ICON_DESTINATION}/icon_16x16.png"
icon_file[1]="${ICON_DESTINATION}/icon_16x16@2x.png"
icon_file[2]="${ICON_DESTINATION}/icon_32x32.png"
icon_file[3]="${ICON_DESTINATION}/icon_32x32@2x.png"
icon_file[4]="${ICON_DESTINATION}/icon_128x128.png"
icon_file[5]="${ICON_DESTINATION}/icon_128x128@2x.png"
icon_file[6]="${ICON_DESTINATION}/icon_256x256.png"
icon_file[7]="${ICON_DESTINATION}/icon_256x256@2x.png"
icon_file[8]="${ICON_DESTINATION}/icon_512x512.png"
icon_file[9]="${ICON_DESTINATION}/icon_512x512@2x.png"

for i in $(seq 0 9); do
sips -z ${iconsizes[$i]} ${iconsizes[$i]} "${SRC_ICON}" --out "${icon_file[$i]}"
done

iconutil -c icns "${ICON_DESTINATION}"

rm -r "${ICON_DESTINATION}"
