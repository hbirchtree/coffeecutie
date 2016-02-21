set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

coffee_add_library ( QtShimCore
    private/qtinit.cpp )

target_link_libraries ( QtShimCore Qt5::Widgets)

add_subdirectory(qt-node-editor)
add_subdirectory(qt-dialogs)
