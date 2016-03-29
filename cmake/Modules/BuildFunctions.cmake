include ( AndroidToolkit )

# Wrappers to get rid of boilerplate and cross-platform-ness (ahem, Android)

macro(TARGET_ENABLE_CXX11 TARGET)
    if(ANDROID)
        #Android's compiler doesn't support target_compile_features :(
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        target_compile_features(${TARGET} PRIVATE cxx_constexpr)
    endif()
endmacro()

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES)
    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}")
    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_enable_cxx11(${TARGET})

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        lib
        )

endmacro()

macro(COFFEE_ADD_LIBRARY TARGET SOURCES)
    # Just a little simplification
    coffee_add_elibrary(${TARGET} ${COFFEE_LINK_OPT} "${SOURCES}")
endmacro()

#Android only uses shared libraries which are loaded, all else uses typical executables
macro(COFFEE_ADD_EXAMPLE TARGET SOURCES)
    if(ANDROID)
        add_library(${TARGET} SHARED ${ANDROID_SDL_MAIN_UNIT} ${SOURCES} )
        set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    else()
        add_executable(${TARGET} ${SOURCES})
    endif()
    target_enable_cxx11(${TARGET})

    install(
        TARGETS
        ${TARGET}

        DESTINATION
        bin
        )
endmacro()