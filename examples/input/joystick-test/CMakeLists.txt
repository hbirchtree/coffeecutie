find_package (SDL2GameControllerDB REQUIRED)

coffee_application(
    TARGET JoystickVisualizer
    TITLE "Joystick Visualizer"
    SOURCES main.cpp
    LIBRARIES CoffeeGLeamRHI CoffeeCore CoffeeRenderer_Default
    RESOURCES
    "${COFFEE_ASSET_DIRECTORY}/input-sprites"
    "${SDL2_GAMECONTROLLERDB}"
    )
