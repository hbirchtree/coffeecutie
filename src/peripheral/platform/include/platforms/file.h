#include "android/file.h"
#include "libc/file.h"
#include "linux/file.h"
#include "posix/file.h"
#include "win32/file.h"

#include "base/file.h"

namespace platform {
namespace file {

using file_error = FileFun::file_error;
}
} // namespace platform
