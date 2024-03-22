# coffee_application ( TARGET BlamTesting TITLE "Blam Loader" SOURCES main.cpp
# LIBRARIES Blam Core Image DropinRenderer GLeamRHI )

find_program ( GLSLANG_PROGRAM glslangValidator )
find_program ( INKSCAPE_PROGRAM inkscape
    PATHS
        /usr
        /usr/local
    PATH_SUFFIXES
        bin
    )
find_package(GameNetworkingSockets CONFIG)

add_custom_target (
  BlamGraphics.Assets
  COMMAND
    ${CMAKE_SOURCE_DIR}/toolchain/desktop/resource_compile.py
    #
    --path ${CMAKE_CURRENT_SOURCE_DIR}/graphics_data
    #
    --output ${CMAKE_CURRENT_BINARY_DIR}/assets
    #
    --cache ${CMAKE_CURRENT_BINARY_DIR}/asset_cache
    #
    --program ShaderCooker=${HOST_TOOLS_BINARY_DIR}/ShaderCooker
    #
    --program Inkscape=${INKSCAPE_PROGRAM}
    #
    --program glslang=${GLSLANG_PROGRAM}
    #
    --program TextureCompressor=${HOST_TOOLS_BINARY_DIR}/TextureCompressor
    #
    --program glslang=${GLSLANG_PROGRAM}
    #
    --target ${CMAKE_SYSTEM_NAME}
    #
    --build-mode Release
#    --build-mode ${CMAKE_BUILD_TYPE}
)

set(BUNDLED_LIBS)
# OpenAL has to have shared linkage
if(BUILD_OPENAL)
    if(ANDROID)
        find_package(OpenSL REQUIRED)
    endif()
    find_package(OpenAL CONFIG REQUIRED)
    get_target_property(OPENAL_LIB OpenAL::OpenAL IMPORTED_LOCATION_DEBUG)
    set(BUNDLED_LIBS
        $<IF:$<CONFIG:Debug>,$<TARGET_PROPERTY:OpenAL::OpenAL,IMPORTED_LOCATION_DEBUG>,$<TARGET_PROPERTY:OpenAL::OpenAL,IMPORTED_LOCATION_RELEASE>>
    )
endif()

coffee_application(
  TARGET BlamGraphics
  TITLE "Blam Graphics"
  SOURCES
        caching.cpp
        graphics.cpp
        map_loading.cpp
        networking.cpp
        occluder.cpp
        rendering.cpp
        resource_creation.cpp
        script_component.cpp
        touch_overlay.cpp
        ui.cpp
        widgets.cpp
  LIBRARIES Blam Core Image GLeamRHI ImGui OAF
  BUNDLE_LIBRARIES ${BUNDLED_LIBS}
  SCHEMES blam halo
  ICON_ASSET ${CMAKE_CURRENT_SOURCE_DIR}/icon_asset.svg
  RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
add_dependencies(BlamGraphics BlamGraphics.Assets)

#target_precompile_headers(
#  BlamGraphics PRIVATE
#  # $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/caching.h>
#  # $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/components.h>
#)
set_target_properties(
  BlamGraphics
  PROPERTIES EMSCRIPTEN_THREADPOOL_SIZE 3
             EMSCRIPTEN_GROWABLE_MEMORY OFF
             EMSCRIPTEN_HEAP_SIZE 1024
)

if(TARGET ASIO)
  target_link_libraries(BlamGraphics PUBLIC ASIO)
endif()
if(TARGET DiscordLatte)
    target_link_libraries(BlamGraphics PUBLIC DiscordLatte)
endif()
if(GameNetworkingSockets_FOUND)
    target_link_libraries(BlamGraphics PUBLIC
        GameNetworkingSockets::GameNetworkingSockets_s
    )
    target_compile_definitions(BlamGraphics PUBLIC -DUSE_NETWORKING=1)
endif()

# coffee_application(
#   TARGET BlamScripting
#   TITLE "Blam Scripting"
#   SOURCES scripting.cpp
#   LIBRARIES Blam Core
# )
