#pragma once

#include <peripherals/identify/system.h>

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID) ||    \
    defined(COFFEE_APPLE) || defined(COFFEE_EMSCRIPTEN) || \
    defined(COFFEE_MINGW64) || defined(COFFEE_MINGW32)
#define USE_POSIX_API 1
#endif

#if defined(COFFEE_ANDROID)
#include "android/fsio.h"
#include "android/mmio.h"
#include "android/rdwrio.h"
#endif

#if defined(COFFEE_EMSCRIPTEN)
#include "emscripten/mmio.h"
#endif

#if defined(COFFEE_MINGW64) || defined(COFFEE_MINGW32)
#include "cpp/fsio.h"
#include "posix/fsio.h"
#include "posix/rdwrio.h"
#include "win32/fsio.h"
#include "win32/mmio.h"
#elif defined(USE_POSIX_API)
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
#define PLATFORM_FILE_SUPPORTS_FS 1
#define PLATFORM_FILE_SUPPORTS_LIST 1
#define PLATFORM_FILE_SUPPORTS_MAPPING 1

using android::open_file;
using android::read;
using android::write;

using android::map;
using android::unmap;

using android::exists;
using android::file_info;
using android::size;
using android::truncate;
using posix::create;
using posix::create_directory;
using posix::remove;

using android::list;

using common::posix::error_message;
#elif defined(COFFEE_MINGW64) || defined(COFFEE_MINGW32)
#define PLATFORM_FILE_SUPPORTS_FS 1
#define PLATFORM_FILE_SUPPORTS_LIST 1
#define PLATFORM_FILE_SUPPORTS_MAPPING 1

using posix::open_file;
using posix::read;
using posix::write;

using win32::map;
using win32::unmap;

using cpp::create;
using cpp::create_directory;
using posix::exists;
using posix::file_info;
using posix::remove;
using posix::size;
using posix::truncate;

using posix::list;

using common::posix::error_message;
#elif defined(USE_POSIX_API)
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

#if !defined(COFFEE_MINGW64) && !defined(COFFEE_MINGW32)
using libc::read_lines;
#endif

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

using file::posix::path::current_dir;
#if defined(COFFEE_MINGW64) || defined(COFFEE_MINGW32)
using file::win32::path::app_dir;
using file::win32::path::executable;
#else
using file::posix::path::app_dir;
using file::posix::path::executable;
#endif
#else
#define PLATFORM_FILE_SUPPORTS_LINK 0
#endif

} // namespace platform::path
