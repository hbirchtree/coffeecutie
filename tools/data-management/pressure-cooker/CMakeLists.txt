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
