find_package (SDL2GameControllerDB REQUIRED)

coffee_add_application_LONG( JoystickVisualizer
    "Joystick Visualizer"
    main.cpp
    "CoffeeSDL2;CoffeeImage"
    ""
    "${COFFEE_ASSET_DIRECTORY}/input-sprites;${SDL2_GAMECONTROLLERDB}" )
