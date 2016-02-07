set(NodeEdit_SRCS private/dialogs.cpp)

add_library(CoffeeQtDialogs STATIC ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtDialogs Qt5::Widgets)

target_enable_cxx11(CoffeeQtDialogs)
