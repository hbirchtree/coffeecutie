#Point Cloud Library
add_subdirectory(pcl-shim)

#The Qt library, includes some MOC and QRC things, some compilation flags required. See the NodeEditorTest example for an example of it running.
add_subdirectory(qt-shim)

#Retro frontend support, GPL code, might be tricky to implement
add_subdirectory(retro-shim)

#Freenect2 binding
add_subdirectory(kinect-shim)
