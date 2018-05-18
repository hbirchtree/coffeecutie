# Add subdir for core modules
add_subdirectory(coffee)

# Add subdir for generation of language bindings
add_subdirectory(bindings)

# Add subdir for extensions, weak bindings of libraries
#add_subdirectory(ext)

# Extension include directories
#set (
#    COFFEE_EXT_PCL_INCLUDE_DIR
#    ${CMAKE_SOURCE_DIR}/ext/pcl-shim/include
#    )
#set (
#    COFFEE_EXT_FREENECT2_INCLUDE_DIR
#    ${CMAKE_SOURCE_DIR}/ext/kinect-shim/include
#    ${COFFEE_CORE_INCLUDE_DIR}
#    )
