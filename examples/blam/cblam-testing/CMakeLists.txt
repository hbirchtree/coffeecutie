set(BLAM_HALO_VERSION "pc_version_t" CACHE STRING "")
set(BLAMG_EMBEDDED_REPORT_URL "" CACHE STRING "")

find_package(GameNetworkingSockets CONFIG)
include(ResourcePackaging)

set(BUNDLED_LIBS)
# OpenAL has to have shared linkage
if(BUILD_OPENAL AND NOT EMSCRIPTEN)
    if(ANDROID)
        find_package(OpenSL REQUIRED)
    endif()
    find_package(OpenAL CONFIG REQUIRED)
    get_target_property(OPENAL_LIB OpenAL::OpenAL IMPORTED_LOCATION_DEBUG)
    set(BUNDLED_LIBS
        $<IF:$<CONFIG:Debug>,$<TARGET_PROPERTY:OpenAL::OpenAL,IMPORTED_LOCATION_DEBUG>,$<TARGET_PROPERTY:OpenAL::OpenAL,IMPORTED_LOCATION_RELEASE>>
    )
elseif(EMSCRIPTEN)
    set(BUNDLED_LIBS OpenAL_ImaDecoder)
endif()

resource_dir_package(
    TARGET BlamGraphics.assets
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/graphics_data
)
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
        server_browser.cpp
        sounds.cpp
        touch_overlay.cpp
        ui.cpp
        widgets.cpp
  LIBRARIES Blam Core Image GLeamRHI ComponentBundleSetup
  BUNDLE_LIBRARIES ${BUNDLED_LIBS}
  SCHEMES blam halo
  ICON_ASSET ${CMAKE_CURRENT_SOURCE_DIR}/icon_asset_plain.svg
  RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
add_dependencies(BlamGraphics BlamGraphics.assets)

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
target_compile_definitions(BlamGraphics
    PUBLIC
    -DBLAM_HALO_VERSION=${BLAM_HALO_VERSION}
    -DEMBEDDED_REPORT_URL=\"${BLAMG_EMBEDDED_REPORT_URL}\"
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
if(TARGET ImGui)
    target_link_libraries(BlamGraphics PUBLIC ImGui)
endif()
if(TARGET OAF)
    target_link_libraries(BlamGraphics PUBLIC OAF)
endif()

# coffee_application(
#   TARGET BlamScripting
#   TITLE "Blam Scripting"
#   SOURCES scripting.cpp
#   LIBRARIES Blam Core
# )
