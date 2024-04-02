find_package(Vorbis CONFIG REQUIRED)

coffee_library(
    TARGET OggDecoder
    SOURCES private/ogg_decode.cpp
    LIBRARIES Core Vorbis::vorbisfile
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

coffee_library(
    TARGET OAF
    SOURCES private/api.cpp
    LIBRARIES ComponentApp OggDecoder
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

if(EMSCRIPTEN)
    coffee_library(
        TARGET ImaDecoder
        LINKAGE SHARED
        SOURCES private/ima_decode.cpp
        LIBRARIES Core
        HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    target_compile_definitions(ImaDecoder PUBLIC
        -DOAF_IMA_DECODER_ENABLED=1
    )
    target_link_libraries(OAF PUBLIC
        ImaDecoder
    )
endif()

if(EMSCRIPTEN)
    # Emscripten requires that we link this one instead
    # It invokes the built-in OpenAL support on top of WebAudio
    target_link_options(OAF PUBLIC -lopenal)
else()
    # Otherwise we use openal-soft
    if(ANDROID)
        find_package(OpenSL REQUIRED)
    endif()
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(OAF PUBLIC OpenAL::OpenAL)
endif()
