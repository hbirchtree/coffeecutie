find_package(assimp REQUIRED)

coffee_library (
    TARGET CoffeeAssimp
    SOURCES
    private/cassimpimporters.cpp
    
    LIBRARIES CoffeeCore Assimp
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/assimp
    )

coffee_bundle_includes ( ${ASSIMP_INCLUDE_DIR} )
coffee_bundle_library ( ${ASSIMP_LIBRARIES} )

if(DEFINED IRRXML_LIBRARIES)
    coffee_bundle_library ( ${IRRXML_LIBRARIES} )
endif()

target_include_directories ( CoffeeAssimp
    PUBLIC
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}/..>
    $<INSTALL_INTERFACE:include/>
    )
