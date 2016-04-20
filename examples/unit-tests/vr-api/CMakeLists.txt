set ( Test_Libraries
    CoffeeCore
    CoffeeVR
    )

coffee_add_example ( Unit_VR_OpenVR_Test01 "Cu_OpenVRTest" test01.cpp "${Test_Libraries}" )
