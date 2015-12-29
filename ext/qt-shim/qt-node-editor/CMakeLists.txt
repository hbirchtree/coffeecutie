set(NodeEdit_SRCS private/node-edit.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(CoffeeQtNodeEditor STATIC ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtNodeEditor Qt5::Widgets)
