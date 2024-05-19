add_link_options(
  #[[-static-libstdc++ -static-libgcc]]
  -Wl,-rpath-link,../lib
  -fuse-ld=gold
  -fuse-linker-plugin
  -flto
)
if(CMAKE_BUILD_TYPE MATCHES "Deb")
  add_compile_options(
    -gsplit-dwarf
    -gdwarf-4
  )
  add_link_options(
    # -Wl,--gdb-index
  )
endif()

find_program(DWP_TOOL ${TOOLCHAIN_TOOL}dwp)
