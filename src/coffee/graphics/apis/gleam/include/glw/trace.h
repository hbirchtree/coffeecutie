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

/* Reads back the finished frame. Must be called just before the swap: on web
 * the drawing buffer is cleared once the browser composites, so by the next
 * frame boundary there is nothing left to read. */
void frame_capture();

/* KHR_debug carries the engine's own names for scopes and objects, and WebGL
 * has no KHR_debug, so gleam::debug drops all of it there. These take the same
 * information into the trace instead, which is the only place it can land. */
void push_group(std::string_view name);
void pop_group();
void label_object(u32 identifier, u32 handle, std::string_view name);
void insert_message(std::string_view text, u32 severity);

/* Pushes on construction and pops on destruction, for gleam::debug::scope. */
struct group_scope
{
    explicit group_scope(std::string_view name)
        : m_active(enabled(level::calls) && !name.empty())
    {
        if(m_active)
            push_group(name);
    }
    ~group_scope()
    {
        if(m_active)
            pop_group();
    }

    group_scope(group_scope const&)            = delete;
    group_scope& operator=(group_scope const&) = delete;

    bool m_active;
};

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

/* How often the colour buffer is read back. Per draw is expensive on purpose:
 * every draw costs a full readback and a sync. */
enum class capture : u8
{
    off = 0,
    frame, /* once per frame, at the swap */
    draw,  /* after every draw call as well */
};

/* Reads back the bound draw framebuffer, downscaled to fit max_edge. Supplied
 * by the RHI for the same reason as the other probes. */
using framebuffer_probe_t =
    bool (*)(u32 max_edge, u32& width, u32& height, std::vector<u8>& out);
void set_framebuffer_probe(framebuffer_probe_t probe);

/* A null base with a non-zero length is normal here: buffer allocations pass
 * no pointer but a real size, and several "pointers" are buffer offsets. A
 * gsl::span cannot hold that pair, so this carries it instead. */
struct data_ref
{
    const char* m_data{nullptr};
    std::size_t m_size{0};

    const char* data() const
    {
        return m_data;
    }
    std::size_t size() const
    {
        return m_size;
    }
};

/* The wrappers hand us several span-like types, not all of which carry
 * size_bytes(), so normalise them here rather than at 1800 call sites. */
template<typename T>
inline data_ref byte_span(T const& v)
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
    return data_ref{ptr, bytes};
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
    string,  /* value is the length; the bytes follow the argument array */
};

struct arg_t
{
    u64      value;
    arg_type type;
    /* Only set for arg_type::string */
    const char* str{nullptr};
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
                          v.data();
                          v.size();
                          requires std::is_same_v<
                              std::remove_cvref_t<decltype(*v.data())>, char>;
                      })
    {
        /* Attribute, uniform and label names. Without this they match the
         * vector branch below and arrive as their first two characters. */
        return {static_cast<u64>(v.size()), arg_type::string, v.data()};
    } else if constexpr(requires {
                          v.size();
                          v[0].data();
                          v[0].size();
                          requires std::is_same_v<
                              std::remove_cvref_t<decltype(*v[0].data())>,
                              char>;
                      })
    {
        /* Shader sources arrive as a list of views. gleam hands over a single
         * chunk, which is the case worth reading; anything else keeps only
         * its count. */
        if(v.size() == 1)
            return {
                static_cast<u64>(v[0].size()),
                arg_type::string,
                v[0].data()};
        return {static_cast<u64>(v.size()), arg_type::opaque};
    } else if constexpr(requires {
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
/* declared_size is what the call moves, which is recorded even when nothing is
 * captured; data/data_size is the copy taken, which is absent below the data
 * level and for calls that only name a size. */
void emit_call(
    const char*  func,
    arg_t const* args,
    u8           argc,
    u32          declared_size,
    const void*  data,
    u32          data_size);

template<typename... Args>
inline void record_call(const char* func, Args&&... args)
{
    if(!enabled(level::calls))
        return;
    if constexpr(sizeof...(Args) == 0)
        emit_call(func, nullptr, 0, 0, nullptr, 0);
    else
    {
        arg_t packed[] = {to_arg(args)...};
        emit_call(
            func, packed, static_cast<u8>(sizeof...(Args)), 0, nullptr, 0);
    }
}

template<typename Span, typename... Args>
inline void record_call_data(const char* func, Span const& data, Args&&... args)
{
    if(!enabled(level::calls))
        return;
    auto const declared = static_cast<u32>(data.size() * sizeof(*data.data()));
    const void* ptr  = nullptr;
    u32         size = 0;
    /* A buffer allocation names a size but hands over no pointer, so the size
     * is worth recording on its own. */
    if(enabled(level::data) && data.data())
    {
        ptr  = static_cast<const void*>(data.data());
        size = declared;
    }
    if constexpr(sizeof...(Args) == 0)
        emit_call(func, nullptr, 0, declared, ptr, size);
    else
    {
        arg_t packed[] = {to_arg(args)...};
        emit_call(
            func,
            packed,
            static_cast<u8>(sizeof...(Args)),
            declared,
            ptr,
            size);
    }
}

} // namespace detail
} // namespace glw::trace
