find_package( Squish QUIET     )
find_package( TIFF QUIET       )
find_package( shaderc QUIET    )
find_package( SPIRVCross QUIET )
find_package( SPIRVTools QUIET )

set ( BUNDLE_LIBS CACHE STRING "" )

if(SQUISH_FOUND)
    list ( APPEND BUNDLE_LIBS
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}PressurizeTextures${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
endif()

if(COFFEE_BUILD_ASSIMP)
    list ( APPEND BUNDLE_LIBS
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}PressurizeModels${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
endif()


coffee_application (
    TARGET PressureCooker
    TITLE "Pressure Cooker"
    SOURCES main.cpp
    LIBRARIES CoffeeCore
    BUNDLE_LIBRARIES ${BUNDLE_LIBS}
    )

coffee_application (
    TARGET Depressurize
    TITLE "Depressurizer"
    SOURCES depressure_main.cpp
    LIBRARIES CoffeeCore
    )

if(COFFEE_BUILD_ASSIMP)
    coffee_library (
        TARGET PressurizeModels
        LINKAGE SHARED
        SOURCES model_compress.cpp
        LIBRARIES CoffeeCore CoffeeAssimp
        HEADER_DIRS ${CMAKE_S123OURCE_DIR}/coffee/interfaces
        )

    if(NOT APPLE)
        set_property ( TARGET PressurizeModels
            PROPERTY LINK_FLAGS
            "-U=CoffeeLoader -fvisibility=hidden -Wl,--exclude-libs,ALL"
            )
    endif()

    add_dependencies ( PressureCooker
        PressurizeModels
        )
endif()

if(SQUISH_FOUND)
    coffee_library(
        TARGET PressurizeTextures
        LINKAGE SHARED
        SOURCES texture_compress.cpp
        LIBRARIES CoffeeImage CoffeeCore Squish
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/coffee/interfaces
        )

    if(NOT APPLE)
        set_property ( TARGET PressurizeTextures
            PROPERTY LINK_FLAGS
            "-U=CoffeeLoader -fvisibility=hidden -Wl,--exclude-libs,ALL"
            )
    endif()

    if(TIFF_FOUND)
        target_compile_definitions ( PressurizeTextures
            PRIVATE
            -DHAVE_LIBTIFF
            )

        target_link_libraries ( PressurizeTextures
            PUBLIC
            TIFF::TIFF
            )
    endif()

    add_dependencies ( PressureCooker
        PressurizeTextures
        )
else()
    message ( STATUS "Skipping PressurizeTextures" )
endif()

if(SHADERC_FOUND)
    coffee_library (
        TARGET PressurizeShaders
        LINKAGE SHARED
        SOURCES shader_compress.cpp
        LIBRARIES CoffeeCore shaderc
        HEADER_DIRS ${CMAKE_SOURCE_DIR}/coffee/interfaces
        )

    target_include_directories( PressurizeShaders
        PRIVATE
        ${SPVTOOLS_INCLUDE_DIR_TMP}
        )

    if(NOT APPLE)
        set_property ( TARGET PressurizeShaders
            PROPERTY LINK_FLAGS
            "-U=CoffeeLoader -fvisibility=hidden -Wl,--exclude-libs,ALL"
            )
    endif()

    if(SPVCROSS_FOUND)
        target_compile_definitions ( PressurizeShaders
            PUBLIC
            -DHAVE_SPIRVCROSS
            )
        target_link_libraries ( PressurizeShaders
            PUBLIC
            spvcross
            )

    endif()

    add_dependencies ( PressureCooker
        PressurizeShaders
        )
endif()
