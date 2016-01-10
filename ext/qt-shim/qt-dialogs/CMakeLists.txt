set(NodeEdit_SRCS private/dialogs.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(CoffeeQtDialogs STATIC ${NodeEdit_SRCS})

target_link_libraries(CoffeeQtDialogs Qt5::Widgets)

target_compile_features(CoffeeQtDialogs PRIVATE cxx_variadic_templates)
