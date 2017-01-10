set(NodeEdit_SRCS private/node-edit.cpp)

coffee_add_library(CoffeeQtNodeEditor ${NodeEdit_SRCS} "CoffeeQtShimCore"
    ${CMAKE_CURRENT_SOURCE_DIR}/private
    )
