#Examples of CAudio API through OpenAL and Coffee audio abstraction layer
add_subdirectory (audio/caudio-test)

#Examples of the CBlam API and its import of Blam engine data
add_subdirectory (blam/cblam-testing)

#Examples of core functionality such as dynamic library loading and information about system environment
add_subdirectory (core/dynamic-test)
add_subdirectory (core/env-test)

#Examples of media playback and capture through CFFMedia, powered by FFMPEG
add_subdirectory (media/media-test)

#Examples of graphics rendering through OpenGL with several different techniques in use
add_subdirectory (opengl/ctest)
add_subdirectory (opengl/ctest-hud)
add_subdirectory (opengl/ctest-async-pbo)
add_subdirectory (opengl/ctest-shaderswitch)

#Experimental code
add_subdirectory (experiments/cgb-test)

#Manipulation of point cloud data through PCL
#add_subdirectory (pcl/pcl-test)

#Qt widgets and etc.
add_subdirectory(qt/node-editor-test)
