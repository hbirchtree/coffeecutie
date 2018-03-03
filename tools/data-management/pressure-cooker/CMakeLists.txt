coffee_add_application (
    PressureCooker "Pressure Cooker"
    "main.cpp" "CoffeeCore"
    )


if(COFFEE_BUILD_ASSIMP)
    coffee_add_elibrary (
        PressurizeModels SHARED "model_compress.cpp"
        "CoffeeCore;CoffeeAssimp"
        "${CMAKE_SOURCE_DIR}/coffee/interfaces"
        )

    add_dependencies ( PressureCooker
        PressurizeModels
        )
endif()

find_package( Squish QUIET )

if(SQUISH_FOUND)
    coffee_add_elibrary(
        PressurizeTextures SHARED "texture_compress.cpp"
        "CoffeeCore;CoffeeImage;Squish"
        "${CMAKE_SOURCE_DIR}/coffee/interfaces"
        )

    add_dependencies ( PressureCooker
        PressurizeTextures
        )
else()
    message ( STATUS "Skipping PressurizeTextures" )
endif()
