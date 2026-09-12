#include <glw/trace.h>

#include <platforms/environment.h>

#include <coffee/core/debug/formatting.h>

#include <cstdio>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/websocket.h>
#endif

namespace glw::trace {

std::atomic<u8> active_level{static_cast<u8>(level::off)};

namespace {

/* Wire format, little-endian:
 *   name  : u8 1, u16 id, u16 len, bytes
 *   call  : u8 2, u16 id, u8 argc, argc * (u8 type, u64 value),
 *           u32 error, u32 data_size, data_size bytes
 *   frame : u8 3, u32 index
 *   tex   : u8 4, u32 id, u32 w, u32 h, u32 size, bytes
 */
enum record_kind : u8
{
    kind_name  = 1,
    kind_call  = 2,
    kind_frame = 3,
    kind_tex   = 4,
};

/* Data blobs are the reason a trace gets big; a whole texture per upload is
 * rarely what you want, and the head of it usually answers the question. */
constexpr u32 max_data_bytes = 4096;
/* A texture is dumped whole rather than capped, but only up to this, so a
 * stray 4K texture cannot stall the run */
constexpr u32 max_texture_bytes = 4 * 1024 * 1024;
/* A texture rewritten every frame would otherwise dump every frame */
constexpr u32 max_dumps_per_texture = 4;
/* Flushed whenever the buffer passes this, and at every frame boundary */
constexpr std::size_t flush_threshold = 8 * 1024;
/* Also flushed every so many records, so a trace survives a page that
 * dies without ever reaching a frame boundary */
constexpr u32 flush_every_records = 128;

struct state_t
{
    std::mutex                                mutex;
    std::vector<u8>                           buffer;
    std::unordered_map<const char*, u16>      names;
    u16                                       next_id{0};
    u32                                       frame{0};
    u32                                       since_flush{0};
    u32 (*error_probe)(){nullptr};
    texture_probe_t texture_probe{nullptr};
    bool            in_probe{false};

    /* Textures carry no size of their own that GL ES will tell us about, so
     * the dimensions are taken from the allocation calls as they go past. */
    struct tex_info
    {
        u32 width{0};
        u32 height{0};
    };
    std::unordered_map<u32, tex_info> textures;
    std::unordered_map<u32, u32>      bound_texture; /* target -> id */
    /* A texture is dumped on the first bind after its contents change, not on
     * the first bind at all: that one is usually the bind made in order to
     * upload into it, which would only ever capture an empty texture. */
    std::unordered_set<u32>      dirty;
    std::unordered_map<u32, u32> dump_count;
    FILE* file{nullptr};
#if defined(__EMSCRIPTEN__)
    EMSCRIPTEN_WEBSOCKET_T socket{0};
    bool                   socket_open{false};
#endif
};

state_t& state()
{
    static state_t s;
    return s;
}

template<typename T>
void put(std::vector<u8>& out, T value)
{
    static_assert(std::is_trivially_copyable_v<T>);
    auto const* bytes = reinterpret_cast<u8 const*>(&value);
    out.insert(out.end(), bytes, bytes + sizeof(T));
}

void put_bytes(std::vector<u8>& out, const void* data, u32 size)
{
    auto const* bytes = static_cast<u8 const*>(data);
    out.insert(out.end(), bytes, bytes + size);
}

/* Caller holds the lock */
void transmit(state_t& s)
{
    if(s.buffer.empty())
        return;
#if defined(__EMSCRIPTEN__)
    if(s.socket_open)
    {
        emscripten_websocket_send_binary(
            s.socket,
            const_cast<void*>(static_cast<const void*>(s.buffer.data())),
            static_cast<int>(s.buffer.size()));
        s.buffer.clear();
    }
    /* Before the socket opens, records accumulate rather than being dropped */
#else
    if(s.file)
    {
        std::fwrite(s.buffer.data(), 1, s.buffer.size(), s.file);
        std::fflush(s.file);
        s.buffer.clear();
    }
#endif
}

#if defined(__EMSCRIPTEN__)
EM_BOOL on_socket_open(int, EmscriptenWebSocketOpenEvent const*, void*)
{
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    s.socket_open = true;
    transmit(s);
    return EM_TRUE;
}

EM_BOOL on_socket_error(int, EmscriptenWebSocketErrorEvent const*, void*)
{
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    s.socket_open = false;
    /* Keeping the records would grow without bound with nobody to take them */
    s.buffer.clear();
    active_level.store(static_cast<u8>(level::off), std::memory_order_relaxed);
    return EM_TRUE;
}
#endif

level parse_level(std::string_view value)
{
    if(value == "calls")
        return level::calls;
    if(value == "errors")
        return level::errors;
    if(value == "data")
        return level::data;
    if(value == "textures")
        return level::textures;
    return level::off;
}

} // namespace

void set_texture_probe(texture_probe_t probe)
{
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    s.texture_probe = probe;
}

void set_error_probe(u32 (*probe)())
{
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    s.error_probe = probe;
}

void init()
{
    auto lvl = level::off;
    if(auto requested = platform::env::var("COFFEE_GL_TRACE");
       requested.has_value())
        lvl = parse_level(*requested);

#if defined(__EMSCRIPTEN__)
    /* Setting environment variables before the runtime starts is awkward from
     * a page, so the level can also come from a global. Returned as a code
     * rather than a string to avoid marshalling one across. */
    if(lvl == level::off)
    {
        int code = EM_ASM_INT({
            var v = '';
            if(typeof window !== 'undefined')
            {
                /* ?gl_trace=errors is the usable route in a browser; the
                 * global is for harnesses that run before the page loads */
                try {
                    v = new URLSearchParams(window.location.search)
                            .get('gleamDebug') || '';
                } catch (e) { v = ''; }
                if(!v)
                    v = window.gleamDebug || '';
            }
            return v === 'calls' ? 1 : v === 'errors' ? 2 : v === 'data' ? 3 :
                   v === 'textures' ? 4 : 0;
        });
        lvl = static_cast<level>(static_cast<u8>(code));
    }
#endif

    if(lvl == level::off)
        return;

    auto& s = state();
    {
        std::lock_guard<std::mutex> _(s.mutex);
        s.buffer.reserve(flush_threshold * 2);

#if defined(__EMSCRIPTEN__)
        auto url = platform::env::var("COFFEE_GL_TRACE_URL").value_or("");
        if(url.empty())
        {
            int port = EM_ASM_INT({
                var p = 0;
                if(typeof window !== 'undefined')
                {
                    try {
                        p = parseInt(new URLSearchParams(window.location.search)
                                         .get('gleamDebugPort') || '', 10) | 0;
                    } catch (e) { p = 0; }
                    if(!p)
                        p = window.gleamDebugPort | 0;
                }
                return p || 8099;
            });
            url = "ws://127.0.0.1:" + std::to_string(port);
        }
        if(!emscripten_websocket_is_supported())
            return;
        EmscriptenWebSocketCreateAttributes attrs = {
            url.c_str(), nullptr, EM_TRUE};
        s.socket = emscripten_websocket_new(&attrs);
        if(s.socket <= 0)
            return;
        emscripten_websocket_set_onopen_callback(
            s.socket, nullptr, on_socket_open);
        emscripten_websocket_set_onerror_callback(
            s.socket, nullptr, on_socket_error);
#else
        auto path =
            platform::env::var("COFFEE_GL_TRACE_FILE").value_or("gltrace.bin");
        s.file = std::fopen(path.c_str(), "wb");
        if(!s.file)
            return;
#endif
    }

    active_level.store(static_cast<u8>(lvl), std::memory_order_relaxed);
    Coffee::Logging::cDebug("GL trace: enabled at level {}", static_cast<int>(lvl));
}

void shutdown()
{
    Coffee::Logging::cDebug("GL trace: shutting down");
    auto& s = state();
    active_level.store(static_cast<u8>(level::off), std::memory_order_relaxed);
    std::lock_guard<std::mutex> _(s.mutex);
    transmit(s);
#if defined(__EMSCRIPTEN__)
    if(s.socket > 0)
    {
        emscripten_websocket_close(s.socket, 1000, "shutdown");
        s.socket      = 0;
        s.socket_open = false;
    }
#else
    if(s.file)
    {
        std::fclose(s.file);
        s.file = nullptr;
    }
#endif
}

void frame_boundary()
{
    if(!enabled(level::calls))
        return;
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    put<u8>(s.buffer, kind_frame);
    put<u32>(s.buffer, s.frame++);
    transmit(s);
}

namespace detail {

void emit_call(
    const char*  func,
    arg_t const* args,
    u8           argc,
    const void*  data,
    u32          data_size)
{
    auto& s = state();

    /* The error probe calls back into GL, which is traced in turn */
    if(s.in_probe)
        return;

    u32 error = 0;
    if(enabled(level::errors) && s.error_probe)
    {
        s.in_probe = true;
        error      = s.error_probe();
        s.in_probe = false;
    }

    std::vector<u8> texture_bytes;
    u32             texture_id = 0, texture_w = 0, texture_h = 0;

    {
        std::lock_guard<std::mutex> _(s.mutex);
        std::string_view            name(func);

        if(name == "glBindTexture" && argc >= 2)
        {
            auto target = static_cast<u32>(args[0].value);
            auto id     = static_cast<u32>(args[1].value);
            s.bound_texture[target] = id;

            if(id != 0 && enabled(level::textures) && s.texture_probe &&
               s.dirty.count(id) &&
               s.dump_count[id] < max_dumps_per_texture)
            {
                if(auto known = s.textures.find(id); known != s.textures.end())
                {
                    auto const& info = known->second;
                    if(info.width && info.height &&
                       info.width * info.height * 4u <= max_texture_bytes)
                    {
                        s.dirty.erase(id);
                        s.dump_count[id]++;
                        texture_id = id;
                        texture_w  = info.width;
                        texture_h  = info.height;
                    }
                }
            }
        } else if(
            name == "glTexSubImage2D" || name == "glCompressedTexSubImage2D" ||
            name == "glGenerateMipmap" || name == "glCopyTexSubImage2D")
        {
            auto target = static_cast<u32>(args[0].value);
            if(auto b = s.bound_texture.find(target); b != s.bound_texture.end())
                s.dirty.insert(b->second);
        } else if(
            (name == "glTexStorage2D" || name == "glTexImage2D") && argc >= 5)
        {
            /* glTexStorage2D(target, levels, format, w, h)
             * glTexImage2D(target, level, format, w, h, ...) */
            auto target = static_cast<u32>(args[0].value);
            /* The size is one packed vector argument whose position differs
             * between these two, so take the first vector we find */
            for(u8 i = 1; i < argc; i++)
            {
                if(args[i].type != arg_type::vector2)
                    continue;
                auto width  = static_cast<u32>(args[i].value & 0xFFFFFFFFu);
                auto height = static_cast<u32>(args[i].value >> 32);
                if(!width || !height)
                    break;
                if(auto b = s.bound_texture.find(target);
                   b != s.bound_texture.end())
                {
                    s.textures[b->second] = state_t::tex_info{width, height};
                    s.dirty.insert(b->second);
                }
                break;
            }
        }
    }

    /* Outside the lock: the probe issues GL calls, which are traced in turn */
    if(texture_id != 0)
    {
        s.in_probe = true;
        if(!s.texture_probe(
               static_cast<u32>(args[0].value),
               texture_id,
               texture_w,
               texture_h,
               texture_bytes))
            texture_bytes.clear();
        s.in_probe = false;
    }

    std::lock_guard<std::mutex> _(s.mutex);

    if(!texture_bytes.empty())
    {
        put<u8>(s.buffer, kind_tex);
        put<u32>(s.buffer, texture_id);
        put<u32>(s.buffer, texture_w);
        put<u32>(s.buffer, texture_h);
        put<u32>(s.buffer, static_cast<u32>(texture_bytes.size()));
        put_bytes(
            s.buffer,
            texture_bytes.data(),
            static_cast<u32>(texture_bytes.size()));
    }

    auto it = s.names.find(func);
    if(it == s.names.end())
    {
        auto id = s.next_id++;
        it      = s.names.emplace(func, id).first;
        std::string_view name(func);
        put<u8>(s.buffer, kind_name);
        put<u16>(s.buffer, id);
        put<u16>(s.buffer, static_cast<u16>(name.size()));
        put_bytes(s.buffer, name.data(), static_cast<u32>(name.size()));
    }

    if(data_size > max_data_bytes)
        data_size = max_data_bytes;
    if(!data)
        data_size = 0;

    put<u8>(s.buffer, kind_call);
    put<u16>(s.buffer, it->second);
    put<u8>(s.buffer, argc);
    for(u8 i = 0; i < argc; i++)
    {
        put<u8>(s.buffer, static_cast<u8>(args[i].type));
        put<u64>(s.buffer, args[i].value);
    }
    put<u32>(s.buffer, error);
    put<u32>(s.buffer, data_size);
    if(data_size)
        put_bytes(s.buffer, data, data_size);

    if(s.buffer.size() >= flush_threshold ||
       ++s.since_flush >= flush_every_records)
    {
        s.since_flush = 0;
        transmit(s);
    }
}

} // namespace detail
} // namespace glw::trace
