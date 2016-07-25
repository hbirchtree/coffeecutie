find_package ( Inkscape )

macro( INKSCAPE_RESIZE_SVG TARGET SOURCE MIPSIZE OUTPUT )
    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND "${INKSCAPE_PROGRAM}" -z -w ${MIPSIZE} -h ${MIPSIZE} -e "${OUTPUT}" "${SOURCE}"
        )
endmacro()

macro ( MAGICK_RESIZE_SVG TARGET SOURCE MIPSIZE OUTPUT )
    add_custom_command (
        TARGET ${TARGET}
        PRE_BUILD
        COMMAND
            convert
                "${SOURCE}"
                -scale ${MIPSIZE}x${MIPSIZE}
                -gravity center
                -channel rgba -alpha on
                -extent ${MIPSIZE}x${MIPSIZE}
                "${OUTPUT}"
        )
endmacro()
