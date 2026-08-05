set(BLAM_HALO_VERSION "pc_version_t" CACHE STRING "")

set(CBLAM_TESTING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../cblam-testing)

resource_dir_package(
  TARGET BlamPoseDemo.assets
  SOURCE_DIR ${CBLAM_TESTING_DIR}/graphics_data
)
coffee_application(
  TARGET BlamPoseDemo
  TITLE "Blam Pose Demo"
  SOURCES
        pose_demo_main.cpp
        pose_demo_spawn.cpp
        pose_apply.cpp
        ${CBLAM_TESTING_DIR}/caching.cpp
        ${CBLAM_TESTING_DIR}/resource_creation.cpp
        ${CBLAM_TESTING_DIR}/rendering.cpp
  LIBRARIES Blam Core Image GLeamRHI ComponentBundleSetup
  ICON_ASSET ${CBLAM_TESTING_DIR}/icon_asset_plain.svg
  RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/assets"
  SHELL ${CMAKE_CURRENT_SOURCE_DIR}/pose_demo_shell.html
)
add_dependencies(BlamPoseDemo BlamPoseDemo.assets)

target_include_directories(BlamPoseDemo PRIVATE ${CBLAM_TESTING_DIR})
target_compile_definitions(BlamPoseDemo
    PUBLIC
    -DBLAM_HALO_VERSION=${BLAM_HALO_VERSION}
    -DDISABLE_LOADING_SCREEN=1
)

set_target_properties(
  BlamPoseDemo
  PROPERTIES EMSCRIPTEN_THREADPOOL_SIZE 3
             EMSCRIPTEN_GROWABLE_MEMORY OFF
             EMSCRIPTEN_HEAP_SIZE 1024
)

if(EMSCRIPTEN)
  target_compile_options(BlamPoseDemo PRIVATE
    -sFETCH
  )
  target_link_options(BlamPoseDemo PUBLIC
    -sFETCH
    -sMODULARIZE=1
    -sEXPORT_NAME=createBlamPoseDemo
  )

  # Vendored MediaPipe Pose (Apache-2.0) + Kalidokit (MIT)
  add_custom_command(
    TARGET BlamPoseDemo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
        $<TARGET_FILE_DIR:BlamPoseDemo>/vendor
  )
endif()
