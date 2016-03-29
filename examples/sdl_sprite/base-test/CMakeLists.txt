coffee_add_example ( SDLSpriteTest main.cpp )
target_link_libraries ( SDLSpriteTest
    CoffeeSDL2
    CoffeeImage
    )

if(ANDROID)
    include ( AndroidApkBuild )

    set ( APK_DEPENDENCIES )
    list ( APPEND APK_DEPENDENCIES ${SDL2_LIBRARY} )
    list ( APPEND APK_DEPENDENCIES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libSDLSpriteTest.so)

    package_apk(
        "SDLSpriteTest"
        "Coffee Test"
        "org.coffee.app"
        "1" "1.0"
        "21" "armeabi-v7a"
        "${APK_DEPENDENCIES}" )
endif()