# Proof-of-concept: hit detection, projectiles and lag compensation over GNS.
# Self-contained; drop the add_subdirectory(poc) line to discard it.

if(NOT (BUILD_GNS AND GameNetworkingSockets_FOUND) OR EMSCRIPTEN)
    return()
endif()

# Game rules, physics stand-in and transport, shared by the CLI and the app
add_library(BlamPocCore STATIC
    game.cpp
    gns_session.cpp
    hit_model.cpp
    physics.cpp
    session.cpp
)
target_compile_definitions(BlamPocCore PRIVATE
    COFFEE_COMPONENT_NAME="BlamPocCore"
)
target_link_libraries(BlamPocCore PUBLIC
    Core
    Blam
    GameNetworkingSockets::GameNetworkingSockets_s
    OpenSSL::SSL
    OpenSSL::Crypto
)

coffee_application(
  TARGET BlamPoc
  TITLE "Blam PoC"
  SOURCES main.cpp
  LIBRARIES Core Blam GraphicsDefines BlamPocCore
  USE_CMD
)
# blam_bitm.h wants a graphics backend define; nothing here draws
target_compile_definitions(BlamPoc PRIVATE GLEAM_DUMMY)

# In-app: a "Combat PoC" window in BlamGraphics
target_sources(BlamGraphics PRIVATE app_combat.cpp)
target_link_libraries(BlamGraphics PRIVATE BlamPocCore)
target_compile_definitions(BlamGraphics PRIVATE POC_COMBAT=1)
