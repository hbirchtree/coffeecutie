set(NodeEdit_SRCS private/dialogs.cpp)

coffee_add_library(CoffeeQtDialogs "${NodeEdit_SRCS}" CoffeeQtShimCore
    ${CMAKE_CURRENT_SOURCE_DIR}/private
    )
