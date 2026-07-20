vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO paullouisageneau/datachannel-wasm
    REF 082586745318df870c9c91249f979717079e47e0 # v0.4.0
    SHA512 5ff2a1f36a42a4c97895d5582baba61c732416c9ca3a88e79ebc4435b56b0d69d14cc65217e9598febc30da22846312d591b63465d914a27145f1bdebc8cb838
    HEAD_REF master
    PATCHES
        add-install-rules.patch
        fix-missing-functional-include.patch
)

# datachannel-wasm's own CMakeLists.txt has no install() rules at all --
# it's normally consumed by copying the wasm/ directory straight into
# another CMake project, not via find_package(). The patch adds
# install()/export() rules referencing this template (a genuinely new
# file, not something upstream has) -- same libwma-style "port injects
# its own new file via file(COPY ...)" pattern the gamenetworkingsockets
# overlay port in this repo already uses.
file(COPY
    "${CMAKE_CURRENT_LIST_DIR}/datachannel-wasm-config.cmake.in"
    DESTINATION "${SOURCE_PATH}"
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME datachannel-wasm CONFIG_PATH share/datachannel-wasm)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
