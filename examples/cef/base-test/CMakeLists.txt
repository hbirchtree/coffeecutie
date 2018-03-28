coffee_application (
    TARGET CEF_Test
    TITLE "CEF Test"
    COMPANY "Coffee"
    VERSION_CODE "10"
    INFO_STRING "A test for Chromium Embedded Framework" "© Coffee?"
    SOURCES main.cpp
    LIBRARIES CoffeeCEF
    PERMISSIONS OPENGL BROWSER
    BUNDLE_BINARIES "CCefSlave"
    )
