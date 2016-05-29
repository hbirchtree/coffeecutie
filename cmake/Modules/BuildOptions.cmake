#
# Documentation!
#

# Generates Doxygen documentation, requires Doxygen installed in PATH
option ( COFFEE_DOCS "Generate documentation" OFF )

option ( COFFEE_TESTS "Build unit tests" ON )

option ( COFFEE_BUILD_BOT "Build CMake bot" OFF )

#
# Platform-specific
#

if(ANDROID)
    option ( ANDROID_USE_SDL2_LAUNCH "Use traditional SDL2 Android bindings" ON )
endif()

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

#
#

#
# Build options
#

# Builds as static libraries which are recommended for ease of use
option ( COFFEE_BUILD_STATIC "Build statically linked binaries" ON )

#
#

# Whether to enable the GLES renderer, will replace desktop GL renderer
option ( COFFEE_BUILD_GLES "Build with GLES 3.0 renderer instead of desktop GL" OFF)

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

# Whether we should build FFMPEG-dependent modules, notably media
option ( COFFEE_BUILD_FFMEDIA "Build with FFMPEG library" OFF)
# Whether we should build the legacy GLBinding module for OpenGL abstraction.
option ( COFFEE_BUILD_GLBINDING "Build with GLBinding library" OFF)
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
# Whether we should build with OculusVR support
# Recommended to set OVR_ROOT_DIR and OVR_LIBRARY_RELEASE on CMake configuration
option ( COFFEE_BUILD_OCULUSVR "Build with OculusVR" OFF)

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
