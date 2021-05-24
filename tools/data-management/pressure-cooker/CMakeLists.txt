option ( PRESSURIZE_FIND_TIFF "Look for libtiff" OFF )

# SPIR-V Tools is built with Shaderc
set ( SPVTOOLS_DIR "${SHADERC_DIR}" )

# TODO: Update these dependencies
find_package( squish QUIET     )
#find_package( shaderc QUIET    )
find_package( spirv_cross_cpp QUIET )
find_package( spirv_cross_glsl QUIET )
find_package( spirv_cross_reflect QUIET )
find_package( spirv_cross_util QUIET )
find_package( SPIRV-Tools QUIET )

if(PRESSURIZE_FIND_TIFF)
    find_package( TIFF QUIET )
endif()

option ( BUILD_ETC2COMP "Build with ETC1/ETC2 compression support" OFF )

set ( BUNDLE_LIBS CACHE STRING "" )

set ( PRESSURIZERS )

if(BUILD_ASSIMP)
    list ( APPEND PRESSURIZERS "$<TARGET_FILE:PressurizeModels>" )
endif()

if(SQUISH_FOUND OR BUILD_ETC2COMP)
    list ( APPEND PRESSURIZERS "$<TARGET_FILE:PressurizeTextures>" )
endif()

if(SHADERC_FOUND)
    list ( APPEND PRESSURIZERS "$<TARGET_FILE:PressurizeShaders>" )
endif()

coffee_application (
    TARGET PressureCooker
    USE_CMD
    TITLE "Pressure Cooker"
    SOURCES main.cpp
    LIBRARIES Core CoreVirtFS
    BUNDLE_LIBRARIES ${PRESSURIZERS}
    )

coffee_application (
    TARGET Depressurize
    USE_CMD
    TITLE "Depressurizer"
    SOURCES depressure_main.cpp
    LIBRARIES Core CoreVirtFS
    )

if(BUILD_ASSIMP)
    coffee_library (
        TARGET PressurizeModels
        LINKABLE
        LINKAGE SHARED
        NO_EXPORT
        SOURCES model_compress.cpp
        LIBRARIES Core AssimpTools
        HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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

if(SQUISH_FOUND OR BUILD_ETC2COMP)
    coffee_library(
        TARGET PressurizeTextures
        LINKABLE
        LINKAGE SHARED
        NO_EXPORT
        SOURCES
        texture_compress.cpp
        dxt_compression.h
        texture_common.h
        tiff_decoder.h

        LIBRARIES Image Core squish::squish
        HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

    if(SQUISH_FOUND)
        target_link_libraries ( PressurizeTextures PUBLIC
            squish::squish
            )
    endif()

    if(NOT APPLE)
        set_property ( TARGET PressurizeTextures
            PROPERTY LINK_FLAGS
            "-U=CoffeeLoader -fvisibility=hidden -Wl,--exclude-libs,ALL"
            )
    endif()

    if(PRESSURIZE_FIND_TIFF AND TIFF_FOUND)
        target_compile_definitions ( PressurizeTextures
            PRIVATE
            HAVE_LIBTIFF
            )

        target_link_libraries ( PressurizeTextures
            PUBLIC
            TIFF::TIFF
            )
    endif()

    if(BUILD_ETC2COMP)
        # Etc2Comp's project files are a mess. Let's not.

        target_sources ( PressurizeTextures
            PRIVATE
            etc2comp/EtcLib/Etc/EtcMath.h
            etc2comp/EtcLib/Etc/EtcMath.cpp
            etc2comp/EtcLib/Etc/EtcColor.h
            etc2comp/EtcLib/Etc/EtcImage.cpp
            etc2comp/EtcLib/Etc/EtcFilter.cpp
            etc2comp/EtcLib/Etc/EtcConfig.h
            etc2comp/EtcLib/Etc/EtcColorFloatRGBA.h
            etc2comp/EtcLib/Etc/EtcFilter.h
            etc2comp/EtcLib/Etc/EtcImage.h
            etc2comp/EtcLib/Etc/Etc.h
            etc2comp/EtcLib/Etc/Etc.cpp
            etc2comp/EtcLib/EtcCodec/EtcDifferentialTrys.cpp
            etc2comp/EtcLib/EtcCodec/EtcSortedBlockList.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8A1.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8A1.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_ETC1.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_ETC1.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RG11.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4EncodingBits.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_R11.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGBA8.h
            etc2comp/EtcLib/EtcCodec/EtcDifferentialTrys.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGBA8.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_R11.cpp
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RG11.h
            etc2comp/EtcLib/EtcCodec/EtcIndividualTrys.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4.cpp
            etc2comp/EtcLib/EtcCodec/EtcIndividualTrys.cpp
            etc2comp/EtcLib/EtcCodec/EtcErrorMetric.h
            etc2comp/EtcLib/EtcCodec/EtcSortedBlockList.h
            etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding.h
            )

        target_include_directories ( PressurizeTextures
            PRIVATE
            etc2comp/EtcLib/Etc
            etc2comp/EtcLib/EtcCodec
            )

        target_compile_definitions ( PressurizeTextures
            PRIVATE
            HAVE_ETC2COMP
            )

    endif()

    add_dependencies ( PressureCooker
        PressurizeTextures
        )
else()
    message ( STATUS "Skipping PressurizeTextures" )
endif()

if(TARGET SPIRV-Tools)
    coffee_library (
        TARGET PressurizeShaders
        LINKABLE
        LINKAGE SHARED
        NO_EXPORT
        SOURCES shader_compress.cpp
        LIBRARIES Core SPIRV-Tools
        HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

#    target_include_directories( PressurizeShaders
#        PRIVATE
#        ${SPVTOOLS_INCLUDE_DIR_TMP}
#        )

    if(NOT APPLE)
        set_property ( TARGET PressurizeShaders
            PROPERTY LINK_FLAGS
            "-U=CoffeeLoader -fvisibility=hidden -Wl,--exclude-libs,ALL"
            )
    endif()

    if(TARGET spirv-cross-glsl)
        target_compile_definitions ( PressurizeShaders
            PUBLIC
            -DHAVE_SPIRVCROSS
            )
        target_link_libraries ( PressurizeShaders
            PUBLIC
            spirv-cross-cpp
            spirv-cross-reflect
            spirv-cross-util
            spirv-cross-glsl
            )
    endif()

    add_dependencies ( PressureCooker
        PressurizeShaders
        )
endif()
