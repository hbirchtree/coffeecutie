#coffee_application (
#    TARGET BlamTesting
#    TITLE "Blam Loader"
#    SOURCES main.cpp
#    LIBRARIES
#    Blam Core Image
#    DropinRenderer GLeamRHI
#    )

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
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics_data
    )

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
