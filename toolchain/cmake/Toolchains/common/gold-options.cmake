add_link_options(
  #[[-static-libstdc++ -static-libgcc]]
  -Wl,-rpath-link,../lib
  -fuse-ld=mold
  -fuse-linker-plugin
  -flto
)

find_program(DWP_TOOL ${TOOLCHAIN_TOOL}dwp)
