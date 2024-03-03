# ##############################################################################
# Shorthand for enabling C++11, with less copy-pasta
# ##############################################################################

include(CheckIPOSupported)

macro(TARGET_ENABLE_CXX11 TARGET)
  set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
  set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)
endmacro()

macro(TARGET_ENABLE_LTO TARGET)
  if(NOT ANDROID AND NOT IOS AND NOT MINGW)
    check_ipo_supported(RESULT LTOSupported)

    if(LTOSupported)
      set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
  endif()
endmacro()
