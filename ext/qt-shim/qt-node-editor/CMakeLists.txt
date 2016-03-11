set(NodeEdit_SRCS private/node-edit.cpp)

coffee_add_library(CoffeeQtNodeEditor ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtNodeEditor CoffeeQtShimCore)
