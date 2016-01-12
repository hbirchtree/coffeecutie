# - What is an ext-classified library?
# These libraries have less tight bindings to the framework, and will most likely introduce different paradigms to the engine. (Eg. smart pointers, different containers)
# For any of these to be accepted as core they must be fully obscured and isolated from the Coffee namespace, and not leak any unwanted types. There are also requirements for library size. Any use of bloated libraries is certain not to be accepted. (Boost being one of them)

#Point Cloud Library
add_subdirectory(pcl-shim)

#The Qt library, includes some MOC and QRC things, some compilation flags required. See the NodeEditorTest example for an example of it running.
add_subdirectory(qt-shim)

#Retro frontend support, GPL code is tricky to link with. We need an interface for this.
#add_subdirectory(retro-shim)

#Freenect2 binding, might be promoted to core
add_subdirectory(kinect-shim)
