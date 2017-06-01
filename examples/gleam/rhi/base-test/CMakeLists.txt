project ( GLeamBaseTest )
set ( GLeamBaseTest_SRCS
    main.cpp
	""
    )

list (APPEND USERDATA
    "${COFFEE_ASSET_DIRECTORY}/eye-demo/"
    )

list (APPEND LIBS 
	CoffeeGLeam
	${STANDARDLIBS_LIBRARIES}
	)

include_directories (
	${STANDARDLIBS_INCLUDE_DIR}
	)

coffee_add_application_long ( GLeamBaseTest_RHI "GLeam Basic RHI" "main.cpp;renderer.h;C:/tmp/SDL2-2.0.5/src/main/winrt/SDL_winrt_main_NonXAML.cpp" "${LIBS}" "" "${USERDATA}" )
