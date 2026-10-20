#!/bin/bash
# Dump the format libraries' view of a fixed map set, for diffing before/after a refactor.
# Usage: blam_format_golden.sh <out dir> [target]   (run from the repo root, tools built)
set -u

OUT=${1:?output directory}
TARGET=${2:-desktop:x86_64-buildroot-linux-gnu:multi:rel}
MAPS=${BLAM_MAPS:-/mnt/blam}

run() {
    local tool=$1 file=$2
    shift 2
    mkdir -p "$(dirname "$OUT/$file")"
    timeout 600 ./cb run "$TARGET/$tool" -- "$@" >"$OUT/$file" 2>&1
    echo "exit $?" >>"$OUT/$file"
}

flag() {
    local f=${1#--}
    echo "${f#-}"
}

h1() {
    local version=$1 map=$2 name
    name=h1/$version-$(basename "$(dirname "$map")")-$(basename "$map" .map)
    [ -f "$map" ] || return
    run blam_tag_inspect "$name/list.txt" --halo-version "$version" -l "$map"
    run blam_tag_inspect "$name/check-lookup.txt" --halo-version "$version" --check-lookup "$map"
    run blam_tag_inspect "$name/scenario.txt" --halo-version "$version" --dump-scenario "$map"
    run blam_tag_inspect "$name/player.txt" --halo-version "$version" --dump-player-biped "$map"
    for class in scnr matg mod2 bitm sbsp antr; do
        run blam_tag_inspect "$name/class-$class.txt" --halo-version "$version" -c "$class" "$map"
    done
}

h2() {
    local map=$1 name
    name=h2/$(basename "$(dirname "$map")")-$(basename "$map" .map)
    [ -f "$map" ] || return
    for mode in -l --check-lookup --scenario --anim-check --scripts; do
        run DimeterInfo "$name/$(flag "$mode").txt" "$mode" "$map"
    done
    for class in scnr matg bitm shad mode sbsp hlmt; do
        run DimeterInfo "$name/class-$class.txt" -d -c "$class" "$map"
    done
}

h3() {
    local map=$1 name
    name=h3/$(basename "$map" .map)
    [ -f "$map" ] || return
    for mode in -l --check-lookup --models --bsp --shaders --bitmaps --collision --objects \
        --nodes --lights --zones --player --animations --sounds --decorators --camera-fx; do
        run ElegyInfo "$name/$(flag "$mode").txt" "$mode" "$map"
    done
}

h1 pc "$MAPS/pc/bloodgulch.map"
h1 pc "$MAPS/pc/a10.map"
h1 pc "$MAPS/pc/c20.map"
h1 xbox "$MAPS/xbox/bloodgulch.map"
h1 xbox "$MAPS/xbox/a10.map"
h1 custom "$MAPS/custom/bloodgulch.map"
h1 custom "$MAPS/custom/a10.map"
h1 custom "$MAPS/refined/a10.map"
h1 mcc "$MAPS/mcc/bloodgulch.map"

h2 "$MAPS/halo2xbox/03a_oldmombasa.map"
h2 "$MAPS/halo2xbox/coagulation.map"
h2 "$MAPS/halo2vista/03a_oldmombasa.map"
h2 "$MAPS/halo2vista/coagulation.map"

h3 "$MAPS/halo3xbox/010_jungle.map"
h3 "$MAPS/halo3xbox/guardian.map"
