find_package (SDL2GameControllerDB REQUIRED)

coffee_application(
    TARGET JoystickVisualizer
    TITLE "Joystick Visualizer"
    SOURCES main.cpp
    LIBRARIES GLeamRHI Core RendererDefault
    RESOURCES
    "${COFFEE_ASSET_DIRECTORY}/input-sprites"
    "${SDL2_GAMECONTROLLERDB}"
    )
