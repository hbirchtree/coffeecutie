#pragma once

/* GL call tracing.
 *
 * Every generated wrapper in glw/ already carries GLW_FPTR_TRACE, so this only
 * provides the sink. Records are packed binary and streamed out as they are
 * produced: on web to a WebSocket collector, elsewhere to a file. Nothing is
 * formatted in-process, because formatting per GL call is what made the
 * previous attempt unusable.
 *
 * Enable with COFFEE_GL_TRACE=calls|errors|data. On web, set it through
 * Module.ENV before the runtime starts.
 */

#include <peripherals/libc/types.h>

#include <gsl/span>

#include <atomic>
#include <vector>
#include <cstring>
#include <string_view>
#include <type_traits>

namespace glw::trace {

using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

enum class level : u8
{
    off = 0,
    calls,  /* function and arguments */
    errors, /* + glGetError() after each call, which costs a sync */
    data,     /* + the data spans passed to uploads, capped per record */
    textures, /* + the contents of each texture, once, on its first bind */
};

/* Hot-path gate. Read directly rather than through a function call so the
 * common disabled case is a load and a branch. */
extern std::atomic<u8> active_level;

inline bool enabled(level required)
{
    return active_level.load(std::memory_order_relaxed) >=
           static_cast<u8>(required);
}

/* Reads configuration and opens the transport. Safe to call when disabled. */
void init();
void shutdown();

/* Marks a frame boundary so the collector can group calls. */
void frame_boundary();

/* Supplies the error-reading function. Kept as a hook so this layer does not
 * reach back into GL itself, and so the call can be guarded against recursing
 * through its own tracing. */
void set_error_probe(u32 (*probe)());

/* Reads back a bound texture. There is no glGetTexImage on GL ES or WebGL, so
 * this attaches the texture to a framebuffer and reads it, which only works for
 * colour-renderable formats; the probe returns false otherwise. Supplied by the
 * RHI for the same reason as the error probe. */
using texture_probe_t =
    bool (*)(u32 target, u32 texture, u32 width, u32 height,
             std::vector<u8>& out);
void set_texture_probe(texture_probe_t probe);

/* The wrappers hand us several span-like types, not all of which carry
 * size_bytes(), so normalise them here rather than at 1800 call sites. */
template<typename T>
inline gsl::span<const char> byte_span(T const& v)
{
    const char* ptr   = nullptr;
    std::size_t bytes = 0;
    if constexpr(requires { v.data(); })
    {
        ptr = reinterpret_cast<const char*>(v.data());
        if constexpr(requires { v.size_bytes(); })
            bytes = static_cast<std::size_t>(v.size_bytes());
        else if constexpr(requires { v.size(); })
        {
            /* Some of these hand back void*, where the size is already bytes */
            using elem = std::remove_pointer_t<decltype(v.data())>;
            if constexpr(std::is_void_v<elem>)
                bytes = static_cast<std::size_t>(v.size());
            else
                bytes = static_cast<std::size_t>(v.size()) * sizeof(elem);
        }
    }
    /* Several of these "pointers" are buffer offsets, so a null base with a
     * non-zero length is normal here and would trip the span's contract. */
    if(ptr == nullptr)
        bytes = 0;
    return gsl::span<const char>(ptr, bytes);
}

namespace detail {

/* Argument type tags, so the collector can render a value rather than a word */
enum class arg_type : u8
{
    integer = 0,
    unsigned_integer,
    floating,
    pointer,
    enumeration,
    vector2, /* two components packed into the low and high halves */
    opaque,  /* aggregate we do not decompose */
};

struct arg_t
{
    u64      value;
    arg_type type;
};

template<typename T>
inline arg_t to_arg(T const& v)
{
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr(std::is_enum_v<U>)
        return {
            static_cast<u64>(static_cast<std::underlying_type_t<U>>(v)),
            arg_type::enumeration};
    else if constexpr(std::is_pointer_v<U>)
        return {
            static_cast<u64>(reinterpret_cast<std::uintptr_t>(v)),
            arg_type::pointer};
    else if constexpr(std::is_floating_point_v<U>)
    {
        double d = static_cast<double>(v);
        u64    out{};
        std::memcpy(&out, &d, sizeof(out));
        return {out, arg_type::floating};
    } else if constexpr(std::is_unsigned_v<U>)
        return {static_cast<u64>(v), arg_type::unsigned_integer};
    else if constexpr(std::is_integral_v<U>)
        return {
            static_cast<u64>(static_cast<std::int64_t>(v)), arg_type::integer};
    else if constexpr(requires {
                          v[0];
                          v[1];
                          requires std::is_arithmetic_v<
                              std::remove_cvref_t<decltype(v[0])>>;
                      })
    {
        /* Sizes and offsets arrive as one vector argument, and losing them
         * costs the trace the dimensions of every texture and viewport. Only
         * the first two components are kept, which is all these carry. */
        auto first  = static_cast<libc_types::u32>(static_cast<std::int64_t>(v[0]));
        auto second = static_cast<libc_types::u32>(static_cast<std::int64_t>(v[1]));
        return {static_cast<u64>(first) | (static_cast<u64>(second) << 32),
                arg_type::vector2};
    } else
        return {0, arg_type::opaque};
}

/* `func` must be a string literal: its address interns the name, so the name
 * itself crosses the wire only once. */
void emit_call(
    const char*  func,
    arg_t const* args,
    u8           argc,
    const void*  data,
    u32          data_size);

template<typename... Args>
inline void record_call(const char* func, Args&&... args)
{
    if(!enabled(level::calls))
        return;
    if constexpr(sizeof...(Args) == 0)
        emit_call(func, nullptr, 0, nullptr, 0);
    else
    {
        arg_t packed[] = {to_arg(args)...};
        emit_call(func, packed, static_cast<u8>(sizeof...(Args)), nullptr, 0);
    }
}

template<typename Span, typename... Args>
inline void record_call_data(const char* func, Span const& data, Args&&... args)
{
    if(!enabled(level::calls))
        return;
    const void* ptr  = nullptr;
    u32         size = 0;
    if(enabled(level::data))
    {
        ptr  = static_cast<const void*>(data.data());
        size = static_cast<u32>(data.size() * sizeof(*data.data()));
    }
    if constexpr(sizeof...(Args) == 0)
        emit_call(func, nullptr, 0, ptr, size);
    else
    {
        arg_t packed[] = {to_arg(args)...};
        emit_call(func, packed, static_cast<u8>(sizeof...(Args)), ptr, size);
    }
}

} // namespace detail
} // namespace glw::trace
