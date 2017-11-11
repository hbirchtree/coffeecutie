set ( Test_Libraries
    CoffeeCore
    CoffeeVR
    )

set ( Test_BundleLibs
	
	)

coffee_add_test ( Unit_VR_OpenVR_Test01
    "Cu_OpenVRTest"
    test01.cpp
    "${Test_Libraries}" )
