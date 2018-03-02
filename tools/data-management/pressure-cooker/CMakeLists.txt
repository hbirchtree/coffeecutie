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

if(COFFEE_BUILD_GLEAM_RHI)
    coffee_add_elibrary(
        PressurizeTextures SHARED "texture_compress.cpp"
        "CoffeeRenderer_Default;CoffeeTesting"
        "${CMAKE_SOURCE_DIR}/coffee/interfaces"
        )

    target_include_directories ( PressurizeTextures
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/stb/
        )

    add_dependencies ( PressureCooker
        PressurizeTextures
        )
endif()
