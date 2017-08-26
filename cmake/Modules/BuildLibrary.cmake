# Contains some code for packaging frameworks
include ( MacAppBuild )

macro(COFFEE_ADD_ELIBRARY TARGET LINKOPT SOURCES LIBRARIES HEADER_DIR)
    add_definitions( -DCOFFEE_APPLICATION_LIBRARY )
    file ( GLOB_RECURSE ${TARGET}_HEADERS
#        ${HEADER_DIR}/*.h
#        ${HEADER_DIR}/*.hpp
        ${HEADER_DIR}/*
        )
    source_group ( "${TARGET}_headers" FILES ${ALL_HEADERS} )

    # Because it's hard to write these three commands over and over again
    add_library(${TARGET} ${LINKOPT} "${SOURCES}" "${${TARGET}_HEADERS}")

    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

    if(APPLE)
        set_target_properties( ${TARGET} PROPERTIES MACOSX_RPATH "." )
    elseif(WIN32)
        set_target_properties ( ${TARGET}
            PROPERTIES
            VERSION ${COFFEE_BUILD_STRING}
            SOVERSION 1
            )
    endif()

    target_enable_cxx11(${TARGET})

    target_link_libraries(${TARGET}
        LINK_PUBLIC
        ${LIBRARIES}
        )

    file ( WRITE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET}.link" "${LIBRARIES}" )

    if(ANDROID)
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            "lib/${ANDROID_ABI}"
            )
    else()
        install(
            TARGETS
            ${TARGET}

            DESTINATION
            "lib/${CMAKE_LIBRARY_ARCHITECTURE}"
            )
    endif()

#    install (
#        FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TARGET}.libraries"
#        DESTINATION deps)
endmacro()

macro(COFFEE_ADD_FRAMEWORK
        TARGET LINKOPT
        VERSION_CODE COPYRIGHT COMPANY
        SOURCES HEADER_DIR
        BUNDLE_RSRCS BUNDLE_HDRS
        LIBRARIES BUNDLE_LIBRARIES)
    if(APPLE AND NOT IOS)
        add_definitions( -DCOFFEE_APPLICATION_LIBRARY )
        MACFRAMEWORK_PACKAGE(
            "${TARGET}" "${LINKOPT}"
            "${VERSION_CODE}" "${COPYRIGHT}" "${COMPANY}"
            "${SOURCES}" "${BUNDLE_RSRCS}" "${BUNDLE_HDRS}")
        target_link_libraries(${TARGET}
            PRIVATE
            ${LIBRARIES}
            )
    else()
        coffee_add_elibrary(
            "${TARGET}" "${LINKOPT}"
            "${SOURCES}" "${LIBRARIES}" "${HEADER_DIR}")
    endif()

    target_enable_cxx11(${TARGET})
endmacro()

macro(COFFEE_ADD_LIBRARY TARGET SOURCES LIBRARIES HEADER_DIR)
    # Just a little simplification
    coffee_add_framework(
        ${TARGET} ${COFFEE_LINK_OPT}
        "2.0" "hbirchtree" "hbirchtree"
        "${SOURCES}" "${HEADER_DIR}" "" ""
        "${LIBRARIES}" "")
endmacro()
