option(BUILD_ASSET_TOOLS OFF
       "Build tools for packaging textures, shaders and etc."
)

if(BUILD_ASSET_TOOLS)
  add_subdirectory(data-management/shader-processor)
  add_subdirectory(data-management/texture-compressor)
endif()
if(${CMAKE_SYSTEM} STREQUAL Linux
   OR (APPLE AND NOT IOS)
   OR WIN32
)
  add_subdirectory(crash-recovery)
endif()

# add_subdirectory ( system-info )

# if(TARGET ASIO) add_subdirectory ( natty-puncher ) endif()
