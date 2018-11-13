find_package(assimp REQUIRED)

coffee_library (
    TARGET CoffeeAssimp
    SOURCES
    private/cassimpimporters.cpp
    
    LIBRARIES CoffeeCore Assimp
    HEADER_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}/..>
    $<INSTALL_INTERFACE:include>
    )

coffee_bundle_includes ( ${ASSIMP_INCLUDE_DIR} )
coffee_bundle_library ( ${ASSIMP_LIBRARIES} )

if(DEFINED IRRXML_LIBRARIES)
    coffee_bundle_library ( ${IRRXML_LIBRARIES} )
endif()
