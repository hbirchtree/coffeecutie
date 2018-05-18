#!/bin/bash

# Define INKSCAPE as the path to the Inkscape executable

function inkscape_e()
{
    ${INKSCAPE} -z $@
}

SPEC="$PWD/launch_specs.json"
SVG_IMG="$PWD/../common/logo_big.svg"
OUT_DIR="$PWD/launch_images"

NUM_IMGS="$(cat $SPEC | jq 'length')"

for i in $(seq 0 $(($NUM_IMGS - 1))); do
    data="$(cat $SPEC | jq .[$i])"
    
    w="$(echo $data | jq .width)"
    h="$(echo $data | jq .height)"
    res="$(echo $data | jq .res)"
    variant="$(echo $data | jq .variant -r)"
    scale="$(echo $data | jq .scale -r)"
    
    [[ -z $res ]] && res="${h}h"
    
    fname="Default-Landscape-${w}h@${scale}~${variant}.png"
    
    src_w="$(inkscape_e ${SVG_IMG} -W)"
    src_h="$(inkscape_e ${SVG_IMG} -H)"
    
    aspect="$(($h * 512 / $w))"
    y0="$(( (512 - $aspect) / 2 ))"
    
    aspect="$(($y0 + $aspect))"
    
    inkscape_e "${SVG_IMG}" --export-area="0:$y0:512:$aspect" -w $w -h $h --export-png="${OUT_DIR}/${fname}" 1>/dev/null
    
    echo "
<dict>
  <key>UILaunchImageMinimumOSVersion</key>
  <string>7.0</string>
  <key>UILaunchImageName</key>
  <string>Default-Landscape-${w}h@${scale}</string>
  <key>UILaunchImageOrientation</key>
  <string>Landscape</string>
  <key>UILaunchImageSize</key>
  <string>{$w, $h}</string>
</dict>"
done

