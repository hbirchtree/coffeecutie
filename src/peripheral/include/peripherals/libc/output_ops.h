#pragma once

#include <peripherals/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

#if defined(COFFEE_LINUX)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <stdio.h>

#define DefaultDebugOutputPipe libc::io::io_handles::err
#define DefaultPrintOutputPipe libc::io::io_handles::out

namespace libc {
namespace io {

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

using output_fd = FILE*;

namespace io_handles {

static FILE* out = stdout;
static FILE* err = stderr;
static FILE* in  = stdin;

} // namespace io_handles

/* TODO: Add errno errors here */

template<input_type Input, typename T = char>
requires(Input == input_type::string && std::is_same_v<T, char>)
    //
    FORCEDINLINE std::basic_string<T> get(
        libc_types::szptr size, FILE* strm = io_handles::in)
{
    std::basic_string<T> out;
    out.resize(size);
    fgets(&out[0], C_FCAST<libc_types::szptr>(out.size()), strm);
    return out;
}

template<input_type Input, typename T = char>
requires(Input == input_type::character && std::is_same_v<T, char>)
    //
    FORCEDINLINE char get(output_fd strm = io_handles::in)
{
    return C_FCAST<char>(fgetc(strm));
}

template<
    flush_mode Mode  = flush_mode::instant,
    input_type Input = input_type::string,
    typename T       = char>
requires(Input == input_type::string && std::is_same_v<T, char>)
    //
    FORCEDINLINE void put(output_fd strm, libc_types::cstring output)
{
    fputs(output, strm);
    if constexpr(Mode == flush_mode::instant)
        fflush(strm);
}

template<
    flush_mode Mode,
    input_type Input,
    typename T = char>
requires(Input == input_type::character && std::is_same_v<T, char>)
    //
FORCEDINLINE void put(output_fd strm, char output)
{
    fputc(output, strm);
    if constexpr(Mode == flush_mode::instant)
        fflush(strm);
}

namespace terminal {
namespace buffers {

FORCEDINLINE void clear(output_fd s = io_handles::out)
{
    io::put<flush_mode::instant, input_type::string>(s, "\033[1;1H\033[2J");
}

FORCEDINLINE void swap()
{
    io::put<flush_mode::instant, input_type::string>(
        io_handles::out, "\033[?1049h\033[H");
}

FORCEDINLINE void reset()
{
    io::put<flush_mode::instant, input_type::string>(
        io_handles::out, "\033[?1049l");
}

} // namespace buffers

FORCEDINLINE bool interactive()
{
#if defined(COFFEE_LINUX)
    auto result = ::isatty(::fileno(io_handles::out));
    errno       = 0;
    return result;
#else
    return false;
#endif
}
FORCEDINLINE void wait()
{
    io::get<input_type::character>();
}

FORCEDINLINE std::pair<libc_types::u32, libc_types::u32> size()
{
#if defined(COFFEE_LINUX)
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return {size.ws_col, size.ws_row};
#else
    return {0, 0};
#endif
}

FORCEDINLINE void bell()
{
#if defined(COFFEE_UNIXPLAT)
    printf("\a");
#elif defined(COFFEE_WINDOWS)
    /* TODO: Implement Windows console bell */
#endif
}

} // namespace terminal

} // namespace io
} // namespace libc
