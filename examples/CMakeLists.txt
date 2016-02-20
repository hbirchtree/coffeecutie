#Examples of CAudio API through OpenAL and Coffee audio abstraction layer
if(COFFEE_BUILD_GLBINDING AND COFFEE_BUILD_OPENAL)
    add_subdirectory (audio/caudio-test)
endif()

#Examples of the CBlam API and its import of Blam engine data
add_subdirectory (blam/cblam-testing)

#Examples of core functionality such as dynamic library loading and information about system environment
if(COFFEE_BUILD_GLBINDING)
    add_subdirectory (core/dynamic-test)
endif()
add_subdirectory (core/env-test)

#Examples of media playback and capture through CFFMedia, powered by FFMPEG
if(COFFEE_BUILD_FFMEDIA AND COFFEE_BUILD_GLBINDING)
    add_subdirectory (media/media-test)
endif()

#Examples of graphics rendering through OpenGL with several different techniques in use
if(COFFEE_BUILD_GLBINDING)
    if(COFFEE_BUILD_ASSIMP)
        add_subdirectory (opengl/ctest)
    endif()
    add_subdirectory (opengl/ctest-hud)
    add_subdirectory (opengl/ctest-async-pbo)
    add_subdirectory (opengl/ctest-shaderswitch)
endif()

#Experimental code
add_subdirectory (experiments/cgb-test)

#Manipulation of point cloud data through PCL
if(COFFEE_BUILD_PCL AND COFFEE_BUILD_NECT)
    add_subdirectory (pcl/pcl-test)
endif()

#Qt widgets and etc.
if(COFFEE_BUILD_QT)
    add_subdirectory(qt/node-editor-test)
endif()

#Not going to say.
add_subdirectory(rsread)

#GLeam
add_subdirectory(gleam/base-test)
add_subdirectory(gleam/shadow-test)

#ASIO examples
add_subdirectory(asio/base-client)
#add_subdirectory(asio/base-server)

#Experimental code
add_subdirectory(gleam/raytrace-test)

add_subdirectory(vr/base-test)
