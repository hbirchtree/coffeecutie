################################################################################
# Shorthand for enabling C++11, with less copy-pasta
################################################################################

macro(TARGET_ENABLE_CXX11 TARGET)
    if(NOT ANDROID AND NOT APPLE)
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
        target_compile_features(${TARGET} PRIVATE cxx_relaxed_constexpr )
    endif()

    if(EMSCRIPTEN)
        target_compile_features(${TARGET} PRIVATE cxx_relaxed_constexpr )
    endif()
endmacro()
