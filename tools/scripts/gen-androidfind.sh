#!/bin/bash

readonly SCRIPT_DIR=$(dirname $(readlink -f "${0}"))

source $SCRIPT_DIR/gen-findscript.sh >/dev/null

echo "
set ( COFFEE_INCLUDE_DIR \"\${COFFEE_ROOT_DIR}/include\" CACHE PATH \"\" )

set ( COFFEE_CORE_LIBRARY \"\${COFFEE_ROOT_DIR}/lib/\${ANDROID_ABI}/libCoffeeCore.a;\${COFFEE_ROOT_DIR}/lib/\${ANDROID_ABI}/libAndroidCore.a\" CACHE STRING \"\" )
"

for e in $(seq 1 $num_libs); do
    echo "set ( COFFEE_${libnames[$e]}_LIBRARY \"\${COFFEE_ROOT_DIR}/lib/\${ANDROID_ABI}/libCoffee${soname[$e]}.a\" CACHE STRING \"\" )"
done

echo ""

echo "\
set ( COFFEE_GRAPHICS_COMMON_LIBRARY"

for e in $(seq 0 ${#graphics_libs[@]}); do
    if [[ ! -z ${graphics_libs[$e]} ]]; then
        echo "    \${COFFEE_${graphics_libs[$e]}_LIBRARY}"
    fi
done

echo "\
    CACHE STRING \"\")"

echo "\
set ( COFFEE_GLEAM_COMMON_LIBRARY"

for e in $(seq 0 ${#gleam_libs[@]}); do
    if [[ ! -z ${gleam_libs[$e]} ]]; then
        echo "    \${COFFEE_${gleam_libs[$e]}_LIBRARY}"
    fi
done

echo "\
    CACHE STRING \"\")"
