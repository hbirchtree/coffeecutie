#pragma once

#include <peripherals/identify/system.h>

//#include "android/file.h"
//#include "libc/file.h"
//#include "linux/file.h"
//#include "posix/file.h"
//#include "win32/file.h"

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID) || \
    defined(COFFEE_APPLE) || defined(COFFEE_EMSCRIPTEN)
#define USE_POSIX_API 1
#endif

#if defined(USE_POSIX_API)
#include "posix/fsio.h"
#include "posix/mmio.h"
#include "posix/rdwrio.h"
#elif defined(COFFEE_WINDOWS)
#error Win32 not implemented
#else
#error Unimplemented file I/O
#endif

#include "libc/fsio.h"
#include "libc/rdwrio.h"

namespace platform::file {

#if defined(COFFEE_ANDROID)

#endif

#if defined(USE_POSIX_API)
#define PLATFORM_FILE_SUPPORTS_FS 1
#define PLATFORM_FILE_SUPPORTS_LIST 1
#define PLATFORM_FILE_SUPPORTS_MAPPING 1

using posix::open_file;
using posix::read;
using posix::write;

using posix::map;
using posix::unmap;

using posix::create;
using posix::create_directory;
using posix::exists;
using posix::file_info;
using posix::remove;
using posix::size;
using posix::truncate;

using posix::list;

using common::posix::error_message;
#else
#define PLATFORM_FILE_SUPPORTS_FS 0
#define PLATFORM_FILE_SUPPORTS_LIST 0
#define PLATFORM_FILE_SUPPORTS_MAPPING 0

using libc::read;
using libc::write;

using libc::open_file;
#endif

using libc::read_lines;

using file_handle = declreturntype(open_file)::value_type;
using map_handle  = declreturntype(map)::value_type;

} // namespace platform::file

namespace platform::path {

#if defined(USE_POSIX_API)
static constexpr auto path_separator = '/';
#elif defined(COFFEE_WINDOWS)
static constexpr auto path_separator = '\\';
#else
#error No path separator defined
#endif

#if defined(USE_POSIX_API)
#define PLATFORM_FILE_SUPPORTS_LINK 1

using file::posix::path::change_dir;

using file::posix::path::canon;
using file::posix::path::dereference;

using file::posix::path::base;
using file::posix::path::dir;

using file::posix::path::app_dir;
using file::posix::path::current_dir;
using file::posix::path::executable;
#else
#define PLATFORM_FILE_SUPPORTS_LINK 0
#endif

} // namespace platform::path
