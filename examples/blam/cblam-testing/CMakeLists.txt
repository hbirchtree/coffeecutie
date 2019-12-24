coffee_application (
    TARGET BlamTesting
    TITLE "Blam Loader"
    SOURCES main.cpp
    LIBRARIES
    Blam Core Image
    DropinRenderer GLeamRHI
    )

coffee_application (
    TARGET BlamGraphics
    TITLE "Blam Graphics"
    SOURCES graphics.cpp
    LIBRARIES
    Blam Core Image DropinRenderer GLeamRHI
    )

coffee_application (
    TARGET BlamScripting
    TITLE "Blam Scripting"
    SOURCES scripting.cpp
    LIBRARIES
    Blam Core
    )
