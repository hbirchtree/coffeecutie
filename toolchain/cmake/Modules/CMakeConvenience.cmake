################################################################################
# Shorthand for enabling C++11, with less copy-pasta
################################################################################

if(EMSCRIPTEN)
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" CACHE STRING "" FORCE )
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
    else()
        set_property ( TARGET ${TARGET}
            PROPERTY CXX_STANDARD 17
            )

    endif()
    set_property ( TARGET ${TARGET}
        PROPERTY CXX_STANDARD_REQUIRED ON
        )
endmacro()
