coffee_application (
    TARGET PowerTest
    TITLE "Power Test"
    SOURCES main.cpp

    LIBRARIES
    CoffeeCore
    ${SDL2_LIBRARY}
    ${SDL2_LIBRARIES}
    )
