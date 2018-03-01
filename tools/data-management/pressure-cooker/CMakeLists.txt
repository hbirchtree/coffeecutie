coffee_add_application (
    PressureCooker "Pressure Cooker"
    "main.cpp" "CoffeeCore"
    )


coffee_add_elibrary (
    PressurizeModels SHARED "model_compress.cpp"
    "CoffeeCore;CoffeeAssimp"
    "${CMAKE_SOURCE_DIR}/coffee/interfaces"
    )
