include ( QtIncludes )

set ( BlamViewer_SRCS
    main.cpp
    blamdataviewer.cpp
    blammaphandler.cpp
    )

list ( APPEND LIBRARIES
    CoffeeBlam
    CoffeeQtShimCore
)

coffee_add_application ( BlamDataViewer "Blam Data Viewer" "${BlamViewer_SRCS}" "${LIBRARIES}" )
