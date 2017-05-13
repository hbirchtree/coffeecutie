#
# Documentation!
#

option ( COFFEE_GENERATE_LIBRARIES "Whether any source should be compiled" ON )
option ( GENERATE_PROGRAMS "Whether examples should be built" ON )

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    option ( COFFEE_GENERATE_APPIMAGE "Generate AppImage packages" OFF )
    option ( COFFEE_GENERATE_FLATPAK "Generate Flatpaks" OFF )
    option ( COFFEE_GENERATE_SNAPPY "Generate Snaps" OFF )
endif()

# Generates Doxygen documentation, requires Doxygen installed in PATH
option ( COFFEE_DOCS "Generate documentation" OFF )

option ( COFFEE_TESTS "Build unit tests" ON )

option ( SKIP_HIGHMEM_TESTS "Skip high-memory tests" OFF )
option ( SKIP_LINKAGE_TEST "Skip test of dynamic linking" ON )
option ( SKIP_OPENVR_TEST "Skip tests of OpenVR, it requires a display and hardware" ON )

option ( COFFEE_BUILD_BOT "Build CMake bot" OFF )


#
#

#
# Language bindings
#

option ( COFFEE_BUILD_SWIG_BINDING "Build SWIG bindings" OFF )

# D language bindings, builds a module
option ( COFFEE_DLANG "Build D-language binding" OFF)
# Python bindings
option ( COFFEE_PYTHON "Build Python binding" OFF)
# CSharp bindings
option ( COFFEE_CSHARP "Build C# binding" OFF)
###

# #

#
# Build options
#

# Builds as static libraries which are recommended for ease of use
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

#
#

# Whether to enable the GLES renderer, will replace desktop GL renderer
option ( COFFEE_BUILD_GLES "Build with GLES 3.0 renderer instead of desktop GL" OFF)

if(NOT COFFEE_BUILD_GLES)
    add_definitions("-DCOFFEE_GLEAM_DESKTOP")
endif()
#
#

#
# Core components, without this, you get nothing
#

# Requires OpenAL libraries and headers
option ( COFFEE_BUILD_OPENAL "Build with OpenAL support" ON)
# Requires SDL2 libraries and headers
option ( COFFEE_BUILD_SDL2 "Build with SDL2 windowing and input" ON)
# Requires SDL2 in most cases, unless you love writing window management code
option ( COFFEE_BUILD_GLEAM "Build GLeam rendering API" ON )
# RHI is written on top of GLeam to provide a device interface, giving access to buffers and surfaces.
option ( COFFEE_BUILD_GLEAM_RHI "Build GLeam rendering hardware interface" ON )
# Requires... Nothing. Only core library
option ( COFFEE_BUILD_CBLAM "Build with Blam engine parsing" ON )
# Requires OpenSSL on system. I'm looking at you, Android
option ( COFFEE_BUILD_OPENSSL "Build with SSL support, used for ASIO SSL sockets" ON )

#
#

#
# External libraries
#

# Whether we should build with Assimp support
option ( COFFEE_BUILD_ASSIMP "Build with Assimp loader" OFF)
# Chromium Embedded Framework
option ( COFFEE_BUILD_CEF "Build with CEF" OFF )

#
#

#
# VR
#

# Whether we should build with OpenVR support
option ( COFFEE_BUILD_OPENVR "Build with OpenVR" OFF)

#
#

#
# Extensions
#

# Whether we should build with support for the PCL extension
option ( COFFEE_BUILD_PCL "Build with PCL" OFF)
# Whether we should build with support for the Kinect extension
option ( COFFEE_BUILD_NECT "Build with Kinect support" OFF)
# Whether we should build with support for the Qt extension
option ( COFFEE_BUILD_QT "Build with Qt support" OFF)

###

#if(ANDROID)
#    if(ANDROID_NATIVE_API_LEVEL LESS 17)
#        message ( "-- Disabling GLEAM_RHI, API ${ANDROID_NATIVE_API_LEVEL} doesn't support it" )
#        set ( COFFEE_BUILD_GLEAM_RHI OFF )
#    endif()
#    # Some options are permanently enabled for some platforms
#    set ( COFFEE_BUILD_GLES ON )
#    set ( COFFEE_BUILD_OPENSSL OFF )
#    set ( COFFEE_BUILD_QT OFF ) # Does not work with Coffee as of yet
#    set ( COFFEE_BUILD_NECT OFF )
#    set ( COFFEE_BUILD_PCL OFF )
##    set ( COFFEE_BUILD_GLEAM_RHI OFF ) # Not implemented for GLES, only GL3.3
#endif()
