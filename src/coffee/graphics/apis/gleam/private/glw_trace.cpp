#include <glw/trace.h>

#include <platforms/environment.h>

#include <coffee/core/debug/formatting.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
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
 *           per string argument (u16 len, bytes),
 *           u32 error, u32 declared_size,
 *           if declared_size: u32 captured, captured bytes
 *   frame : u8 3, u32 index
 *   tex   : u8 4, u32 id, u32 w, u32 h, u32 size, bytes
 *   push  : u8 5, u16 len, bytes
 *   pop   : u8 6
 *   label : u8 7, u32 identifier, u32 handle, u16 len, bytes
 *   msg   : u8 8, u32 severity, u16 len, bytes
 *   xlat  : u8 9, u32 program, u32 len, bytes
 */
enum record_kind : u8
{
    kind_name  = 1,
    kind_call  = 2,
    kind_frame = 3,
    kind_tex   = 4,
    kind_push  = 5,
    kind_pop   = 6,
    kind_label = 7,
    kind_msg   = 8,
    kind_xlat  = 9,
};

/* Strings are capped by the same knob as data, since a shader source is both.
 * The length field is 16-bit, so that is the ceiling whatever the knob says. */
u16 string_cap();

/* Data blobs are the reason a trace gets big; a whole texture per upload is
 * rarely what you want, and the head of it usually answers the question. The
 * declared size is recorded whatever this is, so a clipped blob still says how
 * much it stood for. Raise it with gleamDebugBytes to capture whole buffers. */
constexpr u32   default_max_data_bytes = 4096;
std::atomic<u32> max_data_bytes{default_max_data_bytes};

u16 string_cap()
{
    auto cap = max_data_bytes.load(std::memory_order_relaxed);
    return static_cast<u16>(cap > 0xFFFFu ? 0xFFFFu : cap);
}
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
    /* Programs whose translation has already been sent */
    std::unordered_set<u32> translated;
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

    if(auto cap = platform::env::var("COFFEE_GL_TRACE_BYTES"); cap.has_value())
        max_data_bytes.store(
            static_cast<u32>(std::strtoul(cap->c_str(), nullptr, 10)),
            std::memory_order_relaxed);
#if defined(__EMSCRIPTEN__)
    {
        int bytes = EM_ASM_INT({
            var b = 0;
            if(typeof window !== 'undefined')
            {
                try {
                    b = parseInt(new URLSearchParams(window.location.search)
                                     .get('gleamDebugBytes') || '', 10) | 0;
                } catch (e) { b = 0; }
                if(!b)
                    b = window.gleamDebugBytes | 0;
            }
            return b;
        });
        if(bytes > 0)
            max_data_bytes.store(
                static_cast<u32>(bytes), std::memory_order_relaxed);
    }
#endif

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
    Coffee::Logging::cDebug(
        "GL trace: enabled at level {}, capturing up to {} bytes per call",
        static_cast<int>(lvl),
        max_data_bytes.load(std::memory_order_relaxed));
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

namespace {

#if defined(__EMSCRIPTEN__)
/* ANGLE rewrites every shader before the driver sees it, and when a backend
 * misbehaves the rewritten text is the thing worth reading. WEBGL_debug_shaders
 * is the only route to it, and it lives solely in JS. Returns a malloc'd
 * string, or null when the extension or the shader is missing. */
/* ANGLE rewrites every shader before the driver sees it, and when a backend
 * misbehaves the rewritten text is the thing worth reading. WEBGL_debug_shaders
 * is the only route to it, and it lives solely in JS.
 *
 * Asked at link time rather than after the compile: the browser defers
 * compilation, so immediately after glCompileShader the translation is still
 * empty. Returns a malloc'd string, or null when there is nothing to report. */
EM_JS(char*, translated_program_sources, (int program), {
    try {
        var ctx = (typeof GLctx !== 'undefined' && GLctx)
            ? GLctx
            : ((typeof GL !== 'undefined' && GL.currentContext)
                ? GL.currentContext.GLctx : null);
        if (!ctx || typeof GL === 'undefined') return 0;
        var ext = ctx.getExtension('WEBGL_debug_shaders');
        if (!ext) return 0;
        var prog = GL.programs[program];
        if (!prog) return 0;
        var shaders = ctx.getAttachedShaders(prog) || [];
        var out = '';
        for (var i = 0; i < shaders.length; i++) {
            var src = ext.getTranslatedShaderSource(shaders[i]);
            if (!src) continue;
            var kind = ctx.getShaderParameter(shaders[i], ctx.SHADER_TYPE);
            var name = kind === ctx.VERTEX_SHADER ? 'vertex' : 'fragment';
            out += '// ---- ' + name + ' ----\n' + src + '\n';
        }
        if (!out) return 0;
        var len = lengthBytesUTF8(out) + 1;
        var buf = _malloc(len);
        stringToUTF8(out, buf, len);
        return buf;
    } catch (e) {
        return 0;
    }
});
#endif

/* Shared by the four debug records, all of which end in a length-prefixed
 * string. The caller holds the lock. */
void put_string(std::vector<u8>& buffer, std::string_view text)
{
    auto cap = string_cap();
    auto len = static_cast<u16>(text.size() > cap ? cap : text.size());
    put<u16>(buffer, len);
    put_bytes(buffer, text.data(), len);
}

} // namespace

void push_group(std::string_view name)
{
    if(!enabled(level::calls))
        return;
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    put<u8>(s.buffer, kind_push);
    put_string(s.buffer, name);
}

void pop_group()
{
    if(!enabled(level::calls))
        return;
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    put<u8>(s.buffer, kind_pop);
}

void label_object(u32 identifier, u32 handle, std::string_view name)
{
    if(!enabled(level::calls))
        return;
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    put<u8>(s.buffer, kind_label);
    put<u32>(s.buffer, identifier);
    put<u32>(s.buffer, handle);
    put_string(s.buffer, name);
}

void insert_message(std::string_view text, u32 severity)
{
    if(!enabled(level::calls))
        return;
    auto&                       s = state();
    std::lock_guard<std::mutex> _(s.mutex);
    put<u8>(s.buffer, kind_msg);
    put<u32>(s.buffer, severity);
    put_string(s.buffer, text);
}

namespace detail {

void emit_call(
    const char*  func,
    arg_t const* args,
    u8           argc,
    u32          declared_size,
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

#if defined(__EMSCRIPTEN__)
    /* Programs are linked once, and by then every attached shader has been
     * compiled, so this is where a translation exists to be read. */
    if(enabled(level::data) && std::string_view(func) == "glLinkProgram" &&
       argc >= 1)
    {
        auto program = static_cast<u32>(args[0].value);
        bool first   = false;
        {
            std::lock_guard<std::mutex> _(s.mutex);
            first = s.translated.insert(program).second;
        }
        if(first)
        {
            if(char* src = translated_program_sources(static_cast<int>(program)))
            {
                std::string_view            text(src);
                auto                        len = static_cast<u32>(text.size());
                std::lock_guard<std::mutex> _(s.mutex);
                put<u8>(s.buffer, kind_xlat);
                put<u32>(s.buffer, program);
                put<u32>(s.buffer, len);
                put_bytes(s.buffer, text.data(), len);
                std::free(src);
            }
        }
    }
#endif

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

    if(auto cap = max_data_bytes.load(std::memory_order_relaxed);
       data_size > cap)
        data_size = cap;
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
    /* Names are the point of a trace: an attribute or uniform lookup says
     * nothing without them. They follow the fixed-width arguments, in order,
     * each as long as the length already written above. */
    for(u8 i = 0; i < argc; i++)
    {
        if(args[i].type != arg_type::string)
            continue;
        auto cap = string_cap();
        auto len = static_cast<u16>(
            args[i].value > cap ? cap : args[i].value);
        put<u16>(s.buffer, len);
        if(len && args[i].str)
            put_bytes(s.buffer, args[i].str, len);
    }
    put<u32>(s.buffer, error);
    /* The captured length is only written when there is a length to describe,
     * so the majority of calls, which move no data, cost nothing extra. */
    put<u32>(s.buffer, declared_size);
    if(declared_size)
    {
        put<u32>(s.buffer, data_size);
        if(data_size)
            put_bytes(s.buffer, data, data_size);
    }

    if(s.buffer.size() >= flush_threshold ||
       ++s.since_flush >= flush_every_records)
    {
        s.since_flush = 0;
        transmit(s);
    }
}

} // namespace detail
} // namespace glw::trace
