# coffee_application ( TARGET BlamTesting TITLE "Blam Loader" SOURCES main.cpp
# LIBRARIES Blam Core Image DropinRenderer GLeamRHI )

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
    --program Inkscape=inkscape
    #
    --program TextureCompressor=${HOST_TOOLS_BINARY_DIR}/TextureCompressor
    #
    --target ${CMAKE_SYSTEM_NAME}
    #
    --build-mode Release
#    --build-mode ${CMAKE_BUILD_TYPE}
)

coffee_application(
  TARGET BlamGraphics
  TITLE "Blam Graphics"
  SOURCES
        graphics.cpp
        occluder.cpp
        rendering.cpp
        resource_creation.cpp
        touch_overlay.cpp
        widgets.cpp
  LIBRARIES Blam Core Image GLeamRHI ImGui
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
  PROPERTIES EMSCRIPTEN_THREADPOOL_SIZE 2
             EMSCRIPTEN_GROWABLE_MEMORY ON
             EMSCRIPTEN_HEAP_SIZE 512
)

if(TARGET ASIO)
  target_link_libraries(BlamGraphics PUBLIC ASIO)
endif()

coffee_application(
  TARGET BlamScripting
  TITLE "Blam Scripting"
  SOURCES scripting.cpp
  LIBRARIES Blam Core
)
