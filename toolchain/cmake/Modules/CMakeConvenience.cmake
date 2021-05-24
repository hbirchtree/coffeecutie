################################################################################
# Shorthand for enabling C++11, with less copy-pasta
################################################################################

include (CheckIPOSupported)

if(EMSCRIPTEN)
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20" CACHE STRING "" FORCE )
endif()

macro(TARGET_ENABLE_CXX11 TARGET)
    if(BUILD_CPP11)
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 11
            )
    elseif(BUILD_CPP14)
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 14
            )
    elseif(BUILD_CPP17)
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 17
            )
    else()
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 20
            )
    endif()
    set_property ( TARGET ${TARGET}
        PROPERTY CXX_STANDARD_REQUIRED ON
        )
endmacro()

macro(TARGET_ENABLE_LTO TARGET)
    if(NOT ANDROID AND NOT IOS)
        check_ipo_supported ( RESULT LTOSupported )

        if(LTOSupported)
            set_property ( TARGET ${TARGET}
                PROPERTY INTERPROCEDURAL_OPTIMIZATION ON
                )
        endif()
    endif()
endmacro()
