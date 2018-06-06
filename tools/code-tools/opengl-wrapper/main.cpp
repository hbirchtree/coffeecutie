#define GL_VERSION_VERIFY(a, b) 1
#define GL_VERSION_REQ_COMBO(a, b)
#define GL_VERSION_REQ_DESKTOP(a)
#define GL_VERSION_REQ_ES(a)

#include <stdint.h>

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;


using CGenum = unsigned int;
using c_cptr = const void*;
using c_ptr = void*;
using ptroff = int;
using szptr = unsigned long;
using bigscalar = double;
using scalar = float;
using cstring = const char*;
using cstring_w = char*;
using cstringconst = cstring const;

#include "../../../toolchain/internal/include/glad/glad.h"
#include "source.h"

int main() {}
