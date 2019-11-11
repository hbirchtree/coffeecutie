dependency_get (
    REQUIRED
    SOURCE hbirchtree/native-library-bundle
    TAG ${NATIVE_LIB_BUNDLE_TAG}
    NAMES assimp
    SIDELOAD "${assimp_LOCAL_BUILD}"
    )

find_package(assimp REQUIRED)

coffee_library (
    TARGET AssimpTools
    SOURCES
    private/cassimpimporters.cpp
    
    LIBRARIES Core Assimp CoreObject CoreCompression
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:targets/AssimpTools/libs/include>
    )

dependency_link (
    TARGET AssimpTools
    DEPENDENCIES
        assimp
    )
