cmake_minimum_required(VERSION 3.0)
project ( pvrtcdec LANGUAGES CXX )

add_library( pvrtcdec STATIC )

target_sources( pvrtcdec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/PVRCore/texture/PVRTDecompress.cpp
    )

install( TARGETS pvrtcdec EXPORT pvrtcdec ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install( TARGETS pvrtcdec LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install( FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/PVRCore/texture/PVRTDecompress.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/pvrtcdec
    )

include( CMakePackageConfigHelpers )
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pvrtcdecConfigVersion.cmake
    VERSION 5.9
    COMPATIBILITY SameMinorVersion
    )

export( EXPORT pvrtcdec
    FILE ${CMAKE_CURRENT_BINARY_DIR}/pvrtcdecTargets.cmake
    NAMESPACE pvrtcdec::
    )
install( EXPORT pvrtcdec DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pvrtcdec )
install( EXPORT pvrtcdec 
    FILE pvrtcdecTargets.cmake 
    NAMESPACE pvrtcdec:: 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pvrtcdec
    )
install( FILES 
    pvrtcdecConfig.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/pvrtcdecConfigVersion.cmake 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pvrtcdec
    )
