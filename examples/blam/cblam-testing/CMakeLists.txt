#coffee_application (
#    TARGET BlamTesting
#    TITLE "Blam Loader"
#    SOURCES main.cpp
#    LIBRARIES
#    Blam Core Image
#    DropinRenderer GLeamRHI
#    )
include ( AssetPreprocessor )

#coffee_process_assets(
#    TARGET BlamGraphics.Assets
#    INPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/graphics_data"
#    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/assets"
#    SHADER_VERSIONS
#        "es/300"
#        "es/320"
#        "core/410"
#        "core/430"
#        "core/460"
#        "spv"
#    )
#add_dependencies ( BlamGraphics.Assets ShaderCooker )
add_custom_target( BlamGraphics.Assets
    COMMAND
        ${CMAKE_SOURCE_DIR}/toolchain/desktop/resource_compile.py
            -p ${CMAKE_CURRENT_SOURCE_DIR}/graphics_data
            -o ${CMAKE_CURRENT_BINARY_DIR}/assets
            -P ShaderCooker=${HOST_TOOLS_BINARY_DIR}/ShaderCooker
    )

coffee_application (
    TARGET BlamGraphics
    TITLE "Blam Graphics"
    SOURCES
        graphics.cpp

        caching.cpp
        rendering.cpp
        resource_creation.cpp
        widgets.cpp
    LIBRARIES
    Blam Core Image GLeamRHI ImGui

    RESOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
    )
add_dependencies ( BlamGraphics BlamGraphics.Assets )

target_precompile_headers ( BlamGraphics PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/caching.h>
    $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/components.h>
    )

if(TARGET ASIO)
    target_link_libraries ( BlamGraphics PUBLIC
        ASIO
        )
endif()

coffee_application (
    TARGET BlamScripting
    TITLE "Blam Scripting"
    SOURCES scripting.cpp
    LIBRARIES
    Blam Core
    )
