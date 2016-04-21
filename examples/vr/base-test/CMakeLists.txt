list ( APPEND LIBRARIES
    CoffeeVR
#    CoffeeOVR
    )

coffee_add_example ( VRBaseTest "VR test" main.cpp "${LIBRARIES}" )
