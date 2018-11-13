#pragma once

#include <stdio.h>

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

namespace libc {
namespace io {

using namespace ::libc_types;
using namespace ::stl_types;
using namespace ::type_safety;

enum class input_type
{
    string,
    character
};

enum class flush_mode
{
    instant,
    lazy
};

template<
    input_type Input = input_type::string,
    typename T       = char,
    typename std::enable_if<Input == input_type::string>::type*  = nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
FORCEDINLINE std::basic_string<T> get(szptr size, FILE* strm)
{
    std::basic_string<T> out;
    out.resize(size);
    fgets(&out[0], C_FCAST<szptr>(out.size()), strm);
    return out;
}

template<
    input_type Input,
    typename T                                                   = char,
    typename std::enable_if<Input == input_type::string>::type*  = nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
FORCEDINLINE std::basic_string<T> get(szptr size)
{
    std::basic_string<T> out;
    out.resize(size);
    fgets(&out[0], C_FCAST<szptr>(out.size()), stdin);
    return out;
}

template<
    input_type Input,
    typename T                                                     = char,
    typename std::enable_if<Input == input_type::character>::type* = nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type*   = nullptr>
FORCEDINLINE char get(FILE* strm)
{
    return C_FCAST<char>(fgetc(strm));
}

template<
    input_type Input,
    typename T                                                     = char,
    typename std::enable_if<Input == input_type::character>::type* = nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type*   = nullptr>
FORCEDINLINE char get()
{
    return C_FCAST<char>(getchar());
}

template<
    flush_mode Mode  = flush_mode::instant,
    input_type Input = input_type::string,
    typename T       = char,
    typename std::enable_if<
        Mode == flush_mode::instant && Input == input_type::string>::type* =
        nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
FORCEDINLINE void put(FILE* strm, cstring output)
{
    fputs(output, strm);
    fflush(strm);
}

template<
    flush_mode Mode,
    input_type Input,
    typename T = char,
    typename std::enable_if<
        Mode == flush_mode::lazy && Input == input_type::string>::type* =
        nullptr>
FORCEDINLINE void put(FILE* strm, cstring output)
{
    fputs(output, strm);
}

template<
    flush_mode Mode,
    input_type Input,
    typename T = char,
    typename std::enable_if<
        Mode == flush_mode::instant && Input == input_type::character>::type* =
        nullptr>
FORCEDINLINE void put(FILE* strm, char output)
{
    fputc(output, strm);
    fflush(strm);
}

template<
    flush_mode Mode,
    input_type Input,
    typename T = char,
    typename std::enable_if<
        Mode == flush_mode::lazy && Input == input_type::character>::type* =
        nullptr,
    typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
FORCEDINLINE void put(FILE* strm, char output)
{
    fputc(output, strm);
}

FORCEDINLINE void terminal_wait()
{
    io::get<input_type::character>();
}

} // namespace io
} // namespace libc
