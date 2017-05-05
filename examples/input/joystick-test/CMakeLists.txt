find_package (SDL2GameControllerDB REQUIRED)

include_directories( ${SDL2_INCLUDE_DIR} )

coffee_add_application_LONG( JoystickVisualizer
    "Joystick Visualizer"
    main.cpp
    "CoffeeGLeam"
    ""
    "${COFFEE_ASSET_DIRECTORY}/input-sprites;${SDL2_GAMECONTROLLERDB}" )
