dependency_get (
    REQUIRED
    SOURCE hbirchtree/native-library-bundle
    TAG ${NATIVE_LIB_BUNDLE_TAG}
    NAMES assimp
    )

find_package(assimp REQUIRED)

coffee_library (
    TARGET AssimpTools
    SOURCES
    private/cassimpimporters.cpp
    
    LIBRARIES Core Assimp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:libs/include>
    )

target_include_directories ( AssimpTools PUBLIC
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}/..>
    )

coffee_bundle (
    DESTINATION libs

    HEADER_DIRECTORIES ${ASSIMP_INCLUDE_DIR}
    LIBRARIES ${ASSIMP_LIBRARIES}
    )

if(DEFINED IRRXML_LIBRARIES)
    coffee_bundle (
        DESTINATION libs
        LIBRARIES ${IRRXML_LIBRARIES}
        )
endif()

dependency_link (
    TARGET AssimpTools
    DEPENDENCIES
        assimp
    )
