set(NodeEdit_SRCS private/dialogs.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COFFEE_CORE_INCLUDE_DIR}
    )

add_library(CoffeeQtDialogs STATIC ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtDialogs Qt5::Widgets)

target_enable_cxx11(CoffeeQtDialogs)
