set ( CMAKE_INCLUDE_CURRENT_DIR ON )
set ( CMAKE_AUTOMOC ON )
set ( CMAKE_AUTOUIC ON )

find_package ( Qt5Widgets )
find_package ( Qt5Xml )

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")