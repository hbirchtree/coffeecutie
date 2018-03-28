macro(TARGET_ENABLE_CXX11 TARGET)
    if(NOT ANDROID AND NOT APPLE AND NOT NACL)
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()

    if(EMSCRIPTEN)
        target_compile_features(${TARGET} PRIVATE cxx_relaxed_constexpr )
    endif()
endmacro()
