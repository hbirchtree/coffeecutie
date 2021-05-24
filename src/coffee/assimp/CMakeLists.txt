find_package(assimp CONFIG REQUIRED)

coffee_library (
    TARGET AssimpTools
    SOURCES
    private/cassimpimporters.cpp
    
    LIBRARIES Core assimp::assimp CoreObject CoreCompression
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
