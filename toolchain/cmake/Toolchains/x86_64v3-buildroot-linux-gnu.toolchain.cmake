include(${CMAKE_CURRENT_LIST_DIR}/x86_64-buildroot-linux-gnu.toolchain.cmake)
add_compile_options(
    -march=x86-64-v3
)
