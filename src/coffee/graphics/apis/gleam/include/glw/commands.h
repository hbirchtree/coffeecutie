#pragma once

#include "base.h"

#include "groups.h"

#include <cstddef>
#include <peripherals/concepts/span.h>
#include <peripherals/concepts/vector.h>
#include <peripherals/libc/types.h>
#include <string_view>
#include <tuple>
#include <vector>

#define GLW_FPTR_CHECK(functionName) \
    if constexpr(!gl::is_linked)     \
        if(!gl##functionName)        \
            Throw(undefined_behavior("undefined function " #functionName));

namespace gl {

namespace detail {
inline std::tuple<
    std::vector<GLint>,
    std::vector<const char*>,
    std::vector<std::string>>
transform_strings(std::vector<std::string_view> const& strings)
{
    std::vector<GLint>       string_lengths;
    std::vector<const char*> cstrings;
    std::vector<std::string> stringstorage;

    for(auto const& view : strings)
    {
        stringstorage.push_back(std::string(view.begin(), view.end()));
        cstrings.push_back(stringstorage.back().c_str());
        string_lengths.push_back(stringstorage.back().size());
    }

    return std::make_tuple(
        std::move(string_lengths),
        std::move(cstrings),
        std::move(stringstorage));
}

template<typename T1, typename T2>
inline void assert_equal(T1 const& v1, T2 const& v2)
{
    if(v1 != v2)
        Throw(std::runtime_error("assertion failed"));
}

} // namespace detail

static_assert(std::is_same_v<GLubyte, u8>, "GLubyte does not match u8");
using ::libc_types::u8;
static_assert(std::is_same_v<GLbyte, i8>, "GLbyte does not match i8");
using ::libc_types::i8;
static_assert(std::is_same_v<GLushort, u16>, "GLushort does not match u16");
using ::libc_types::u16;
static_assert(std::is_same_v<GLshort, i16>, "GLshort does not match i16");
using ::libc_types::i16;
static_assert(std::is_same_v<GLuint, u32>, "GLuint does not match u32");
using ::libc_types::u32;
static_assert(std::is_same_v<GLint, i32>, "GLint does not match i32");
using ::libc_types::i32;
#if defined(GL_VERSION_4_1) || defined(GL_ES_VERSION_3_2)
static_assert(std::is_same_v<GLuint64, u64>, "GLuint64 does not match u64");
using ::libc_types::u64;
#endif
#if defined(GL_VERSION_4_1) || defined(GL_ES_VERSION_3_2)
static_assert(std::is_same_v<GLint64, i64>, "GLint64 does not match i64");
using ::libc_types::i64;
#endif
static_assert(std::is_same_v<GLfloat, f32>, "GLfloat does not match f32");
using ::libc_types::f32;
#if defined(GL_VERSION_4_1)
static_assert(std::is_same_v<GLdouble, f64>, "GLdouble does not match f64");
using ::libc_types::f64;
#endif

} // namespace gl

namespace gl::impl {

template<typename Current>
struct core
{
    using version = Current;
#include "versions/ver_10_core.h"
#include "versions/ver_11_core.h"
#include "versions/ver_12_core.h"
#include "versions/ver_13_core.h"
#include "versions/ver_14_core.h"
#include "versions/ver_15_core.h"
#include "versions/ver_20_core.h"
#include "versions/ver_21_core.h"
#include "versions/ver_30_core.h"
#include "versions/ver_31_core.h"
#include "versions/ver_32_core.h"
#include "versions/ver_33_core.h"
#include "versions/ver_40_core.h"
#include "versions/ver_41_core.h"
#include "versions/ver_42_core.h"
#include "versions/ver_43_core.h"
#include "versions/ver_44_core.h"
#include "versions/ver_45_core.h"
#include "versions/ver_46_core.h"

}; // struct core

template<typename Current>
struct es
{
    using version = Current;
#include "versions/ver_20_es.h"
#include "versions/ver_30_es.h"
#include "versions/ver_31_es.h"
#include "versions/ver_32_es.h"

}; // struct es

} // namespace gl::impl
