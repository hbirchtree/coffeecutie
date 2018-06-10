# Whether we should build with support for the PCL extension
option ( BUILD_PCL "Build with PCL" OFF)
# Whether we should build with support for the Kinect extension
option ( BUILD_NECT "Build with Kinect support" OFF)
# Whether we should build with support for the Qt extension
option ( BUILD_QT "Build with Qt support" OFF)

# - What is an ext-classified library?
# These libraries have less tight bindings to the framework, and will most likely introduce different paradigms to the engine. (Eg. smart pointers, different containers)
# For any of these to be accepted as core they must be fully obscured and isolated from the Coffee namespace, and not leak any unwanted types. There are also requirements for library size. Any use of bloated libraries is certain not to be accepted. (Boost being one of them)

#Point Cloud Library
if(BUILD_PCL)
    add_subdirectory(pcl-shim)
endif()

#The Qt library, includes some MOC and QRC things, some compilation flags required. See the NodeEditorTest example for an example of it running.
if(BUILD_QT)
    add_subdirectory(qt-shim)
endif()

#Retro frontend support, GPL code is tricky to link with. We need an interface for this.
#add_subdirectory(retro-shim)

#Freenect2 binding, might be promoted to core
if(BUILD_NECT)
    add_subdirectory(kinect-shim)
endif()
