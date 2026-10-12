find_program(INKSCAPE_PROGRAM inkscape)
find_program(MAGICK_PROGRAM NAMES magick convert)

macro(INKSCAPE_RESIZE_SVG TARGET SOURCE MIPSIZE OUTPUT)
  add_custom_command(
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND "${INKSCAPE_PROGRAM}" -w ${MIPSIZE} -h ${MIPSIZE}
            --export-type=png -o "${OUTPUT}" "${SOURCE}"
  )
endmacro()

macro(MAGICK_RESIZE_SVG TARGET SOURCE MIPSIZE OUTPUT)
  add_custom_command(
    TARGET ${TARGET}
    PRE_BUILD
    COMMAND
      "${MAGICK_PROGRAM}" -background none -scale ${MIPSIZE}x${MIPSIZE} -gravity center
      -channel rgba -alpha on -extent ${MIPSIZE}x${MIPSIZE} "${SOURCE}"
      "${OUTPUT}"
  )
endmacro()

# Rasterizes SVG into Android launcher mipmaps under OUTPUT_DIR/mipmap-*dpi
# Inkscape is used because it renders mesh gradients, which VectorDrawable cannot express
function(SVG_TO_ANDROID_MIPMAPS SOURCE OUTPUT_DIR OUTPUTS_VAR)
  set(outputs)
  # density, legacy icon px (48dp), adaptive canvas px (108dp), safe zone px (66dp)
  foreach(density "mdpi;48;108;66" "hdpi;72;162;99" "xhdpi;96;216;132"
                  "xxhdpi;144;324;198" "xxxhdpi;192;432;264")
    list(GET density 0 name)
    list(GET density 1 legacy)
    list(GET density 2 canvas)
    list(GET density 3 safe)
    set(dir "${OUTPUT_DIR}/mipmap-${name}")
    add_custom_command(
      OUTPUT "${dir}/ic_launcher.png" "${dir}/ic_launcher_round.png"
             "${dir}/ic_launcher_foreground.png"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${dir}"
      COMMAND "${INKSCAPE_PROGRAM}" -w ${legacy} -h ${legacy}
              --export-type=png -o "${dir}/ic_launcher.png" "${SOURCE}"
      COMMAND ${CMAKE_COMMAND} -E copy "${dir}/ic_launcher.png"
              "${dir}/ic_launcher_round.png"
      COMMAND "${INKSCAPE_PROGRAM}" -w ${safe} -h ${safe}
              --export-type=png -o "${dir}/ic_launcher_foreground.png" "${SOURCE}"
      COMMAND "${MAGICK_PROGRAM}" "${dir}/ic_launcher_foreground.png"
              -background none -gravity center -extent ${canvas}x${canvas}
              "${dir}/ic_launcher_foreground.png"
      DEPENDS "${SOURCE}"
      VERBATIM
    )
    list(APPEND outputs "${dir}/ic_launcher.png" "${dir}/ic_launcher_round.png"
         "${dir}/ic_launcher_foreground.png")
  endforeach()
  set(${OUTPUTS_VAR} ${outputs} PARENT_SCOPE)
endfunction()
