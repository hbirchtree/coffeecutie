################################################################################
# Shorthand for enabling C++11, with less copy-pasta
################################################################################

macro(TARGET_ENABLE_CXX11 TARGET)
    if (NOT DEFINED CMAKE_CXX_STANDARD)
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 14
            )
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD_REQUIRED ON
            )
    endif()
endmacro()
