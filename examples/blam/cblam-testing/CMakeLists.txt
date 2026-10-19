set(BLAM_HALO_VERSION "xbox_version_t" CACHE STRING "")
set(BLAMG_EMBEDDED_REPORT_URL "" CACHE STRING "")
set(BLAM_CURSED OFF CACHE STRING "Cursed experiments")

find_package(GameNetworkingSockets CONFIG)
if(EMSCRIPTEN)
    find_package(datachannel-wasm CONFIG)
else()
    find_package(LibDataChannel CONFIG QUIET)
endif()
option(BUILD_PHYSICS "Enable Bullet physics support" ON)
if(BUILD_PHYSICS)
    find_package(Bullet CONFIG QUIET)
endif()
include(ResourcePackaging)

set(BUNDLED_LIBS)
# OpenAL has to have shared linkage
if(BUILD_OPENAL AND NOT EMSCRIPTEN)
    if(ANDROID)
        find_package(OpenSL REQUIRED)
    endif()
    find_package(OpenAL CONFIG REQUIRED)
    set(shared_libs OpenAL::OpenAL)
    if(BUILD_OPENAL_WMA)
        find_package(libwma CONFIG REQUIRED)
        list(APPEND shared_libs libwma::wma)
    endif()
    # Some triplets (Android) still link OpenAL statically
    foreach(lib ${shared_libs})
        get_target_property(lib_type ${lib} TYPE)
        if(lib_type STREQUAL "SHARED_LIBRARY")
            list(APPEND BUNDLED_LIBS $<TARGET_SONAME_FILE:${lib}>)
        endif()
    endforeach()
elseif(BUILD_OPENAL AND EMSCRIPTEN)
    find_package(OpenAL CONFIG REQUIRED)
    set(BUNDLED_LIBS OpenAL::OpenAL)
    if(BUILD_OPENAL_WMA)
        find_package(libwma CONFIG REQUIRED)
        list(APPEND BUNDLED_LIBS libwma::wma)
    endif()
    if(BUILD_OPENAL_XMA2)
        # Imported targets are per directory; OAF's find_package isn't seen here
        find_package(unofficial-ffmpeg-decoders CONFIG REQUIRED)
        list(APPEND BUNDLED_LIBS unofficial::ffmpeg-decoders::ffmpeg-decoders)
    endif()
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
        camera_control.cpp
        gameplay.cpp
        animation_controller.cpp
        impact_sounds.cpp
        vehicle_sounds.cpp
        projectiles.cpp
        damage.cpp
        game_mode.cpp
        campaign_controller.cpp
        multiplayer_controller.cpp
        checkpoint.cpp
        effects.cpp
        weapons.cpp
        hud.cpp
        ticks.cpp
        debug_ui/widgets.cpp
        forge_controller.cpp
        graphics.cpp
        h2/h2_loader.cpp
        h2/h2_occluder.cpp
        h2/h2_renderer.cpp
        h2/h2_shell.cpp
        h2/h2_sound.cpp
        player_controller.cpp
        loading.cpp
        map_loading.cpp
        network/gateway_fleet_registration.cpp
        network/networking.cpp
        network/webrtc_identity.cpp
        network/webrtc_signaling.cpp
        offline_maps.cpp
        physics.cpp
        render/occluder.cpp
        render/rendering.cpp
        resource_creation.cpp
        script_component.cpp
        sounds.cpp
        input/touch_overlay.cpp
        ui.cpp
        ui_game_setup.cpp
        ui_profile.cpp
  LIBRARIES Blam Core Image GLeamRHI ComponentBundleSetup
  BUNDLE_LIBRARIES ${BUNDLED_LIBS}
  SCHEMES blam halo
  ICON_ASSET ${CMAKE_CURRENT_SOURCE_DIR}/icon_asset_plain.svg
  RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
add_dependencies(BlamGraphics BlamGraphics.assets)
# Crunched assets (textures/meshes as constexpr arrays) land here.
target_include_directories(BlamGraphics PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${CMAKE_CURRENT_SOURCE_DIR}
)
# Crunched data lives in its own translation unit, so including the header
# does not drag the initialisers into every consumer
file(GLOB BLAMG_CRUNCHED_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/generated/crunched/*.cpp)
target_sources(BlamGraphics PRIVATE ${BLAMG_CRUNCHED_SOURCES})

#target_precompile_headers(
#  BlamGraphics PRIVATE
#  # $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/caching.h>
#  # $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/components.h>
#)
# Two of the pool go to openal-soft: its mixer and its event thread
set_target_properties(
  BlamGraphics
  PROPERTIES EMSCRIPTEN_THREADPOOL_SIZE 6
             EMSCRIPTEN_GROWABLE_MEMORY OFF
             EMSCRIPTEN_HEAP_SIZE 1024
             APP_PRIMARY_COLOR "#081F49"
             APP_SECONDARY_COLOR "#1F0445"
             APP_GRID_COLOR "#11FFFFFF"
)
target_compile_definitions(BlamGraphics
    PUBLIC
    -DBLAM_HALO_VERSION=${BLAM_HALO_VERSION}
    -DEMBEDDED_REPORT_URL=\"${BLAMG_EMBEDDED_REPORT_URL}\"
)

if(TARGET Net)
  target_link_libraries(BlamGraphics PUBLIC Net)
endif()
if(EMSCRIPTEN)
  # Lists maps uploaded to OPFS by BlamMapUpload before main() runs
  target_link_options(BlamGraphics PUBLIC
    -sFETCH
    --pre-js=${CMAKE_CURRENT_SOURCE_DIR}/offline_maps_preload.js
  )
  set_property(
    TARGET BlamGraphics APPEND
    PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/offline_maps_preload.js
  )
endif()
if(TARGET DiscordLatte)
    target_link_libraries(BlamGraphics PUBLIC DiscordLatte)
endif()
if(BUILD_GNS AND GameNetworkingSockets_FOUND AND
        (WIN32 OR (APPLE AND NOT IOS) OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
)
    # Joins servers on behalf of consoles that only have UDP (GameCube)
    add_subdirectory(
        ${PROJECT_SOURCE_DIR}/tools/blam/gc-gateway
        ${CMAKE_CURRENT_BINARY_DIR}/gc-gateway
    )
endif()
if(BUILD_GNS AND GameNetworkingSockets_FOUND)
    target_link_libraries(BlamGraphics PUBLIC
        GameNetworkingSockets::GameNetworkingSockets_s
    )
    target_compile_definitions(BlamGraphics PUBLIC -DUSE_NETWORKING=1)
    # Peer identity keys and certificate signing (webrtc_identity.cpp) are a
    # GNS-level concern, not a WebRTC one -- a plain UDP or P2P connection
    # authenticates the same way -- so OpenSSL is needed wherever networking
    # is, independently of the DataChannel transport below.
    target_link_libraries(BlamGraphics PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()
if(BUILD_GNS AND
        GameNetworkingSockets_FOUND AND
        (LibDataChannel_FOUND OR EMSCRIPTEN)
)
    if(EMSCRIPTEN)
        target_link_libraries(BlamGraphics PUBLIC
            datachannel-wasm::datachannel-wasm
        )
    else()
        target_link_libraries(BlamGraphics PUBLIC
            LibDataChannel::LibDataChannel
        )
    endif()
    target_link_libraries(BlamGraphics PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
    )
    target_compile_definitions(BlamGraphics PUBLIC -DUSE_WEBRTC_TRANSPORT=1)
endif()
if(TARGET ImGui)
    target_link_libraries(BlamGraphics PUBLIC ImGui)
endif()
if(TARGET OAF)
    target_link_libraries(BlamGraphics PUBLIC OAF)
endif()
if(BUILD_PHYSICS AND BULLET_FOUND)
    target_compile_definitions(BlamGraphics PRIVATE -DFEATURE_ENABLE_BULLET3=1)
    target_link_libraries(BlamGraphics PRIVATE ${BULLET_LIBRARIES})
    target_include_directories(BlamGraphics PRIVATE ${BULLET_INCLUDE_DIR})
endif()

# coffee_application(
#   TARGET BlamScripting
#   TITLE "Blam Scripting"
#   SOURCES scripting.cpp
#   LIBRARIES Blam Core
# )

if(BLAM_CURSED)
    find_package(BZip2 REQUIRED)
    # hash-library: SHA-256 for the RS2 cache content signature
    # (rs2_geometry.h); PeripheralPlatform only links it on Windows.
    find_library(HASH_LIBRARY_LIB hash-library)
    target_sources(BlamGraphics PRIVATE
      cursed.cpp
    )
    target_link_libraries(BlamGraphics PRIVATE
      BZip2::BZip2
      ${HASH_LIBRARY_LIB}
    )
    target_compile_definitions(BlamGraphics PRIVATE
      -DBLAM_CURSED_ENABLED
    )
endif()

# Combat/netcode proof-of-concept (headless BlamPoc)
add_subdirectory(poc)
