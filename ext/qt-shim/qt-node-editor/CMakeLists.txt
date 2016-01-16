set(NodeEdit_SRCS private/node-edit.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COFFEE_CORE_INCLUDE_DIR}
    )

add_library(CoffeeQtNodeEditor ${COFFEE_LINK_OPT} ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtNodeEditor Qt5::Widgets)

target_enable_cxx11(CoffeeQtNodeEditor)
