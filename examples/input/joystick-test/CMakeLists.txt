find_package (SDL2GameControllerDB REQUIRED)

coffee_add_application_LONG( JoystickVisualizer
    "Joystick Visualizer"
    main.cpp
    "CoffeeGLeamRHI;CoffeeCore;CoffeeRenderer_Default"
    ""
    "${COFFEE_ASSET_DIRECTORY}/input-sprites;${SDL2_GAMECONTROLLERDB}" )
