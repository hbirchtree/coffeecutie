coffee_library(
    TARGET OAF
    SOURCES private/api.cpp
    LIBRARIES ComponentApp
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

if(EMSCRIPTEN)
    # Emscripten requires that we link this one instead
    # It invokes the built-in OpenAL support on top of WebAudio
    target_link_options(OAF PUBLIC -lopenal)
else()
    # Otherwise we use openal-soft
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(OAF PUBLIC OpenAL::OpenAL)
endif()
