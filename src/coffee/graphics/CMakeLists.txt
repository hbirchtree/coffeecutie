# GLeam requires no dependencies outside of SDL2
if(COFFEE_BUILD_GLEAM AND COFFEE_BUILD_OPENGL)
    add_subdirectory(apis/gleam)
endif()

add_subdirectory(common)
