################################################################################
# Shorthand for enabling C++11, with less copy-pasta
################################################################################

macro(TARGET_ENABLE_CXX11 TARGET)
    if(NOT ANDROID AND NOT APPLE)
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()

    set_property ( TARGET ${TARGET}
        PROPERTY CXX_STANDARD 14
        )
    set_property ( TARGET ${TARGET}
        PROPERTY CXX_STANDARD_REQUIRED 11
        )

    if(EMSCRIPTEN)
        target_compile_features(${TARGET} PRIVATE cxx_relaxed_constexpr )
    endif()
endmacro()
