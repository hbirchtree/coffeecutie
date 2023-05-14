#include <platforms/profiling/jsonprofile.h>

#include <coffee/core/base_state.h>
#include <coffee/core/coffee.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>
#include <platforms/file.h>

#include <atomic>

#include <coffee/core/profiler/profiling-export.h>

#include <coffee/strings/format.h>

#if defined(COFFEE_ANDROID)
#include <android/trace.h>
#endif

namespace platform {
namespace profiling {
namespace json {

using namespace ::platform::file;
using semantic::Bytes;
using semantic::BytesConst;
using semantic::RSCA;

static constexpr cstring event_format =
    R"({{"ts":{0},"name":"{1}","pid":1,"tid":{2},"cat":"{3}","ph":"{4}","s":"t"}},
)";

struct MetricData
{
    MetricVariant variant;
    u32           id;
};

namespace metrics {
static std::map<std::string_view, MetricData> data;
static u32                                    count;
} // namespace metrics

struct ProfileWriter : GlobalState
{
    ProfileWriter(Url outputProfile)
    {
        if constexpr(!compile_info::profiler::enabled)
            return;

        /* We keep a reference to this pointer in order to extend its lifespan.
         * In the destructor we need it for finalizing the thread names. */
        threadState = state->PeekState("threadNames");
        appData     = state->GetAppData();
        if(!threadState)
            Throw(undefined_behavior("thread naming is not initialized!"));

        if(auto fd = platform::file::open_file(
               outputProfile,
               RSCA::Append | RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
           fd.has_value())
            logfile = std::move(fd.value());

        if(auto error = platform::file::write(
               logfile,
               mem_chunk<const char>::ofString(
                   R"({"displayTimeUnit": "ms","traceEvents":[)"));
           error.has_value())
            return;
        if(auto disable = platform::env::var("COFFEE_DISABLE_PROFILER");
           disable.has_value() && disable.value() == "1")
            disable_frequent = true;
        disable_frequent = true;
    }

    declreturntype(platform::file::open_file)::value_type logfile;
    std::shared_ptr<GlobalState>             threadState;
    std::shared_ptr<platform::info::AppData> appData;
    std::atomic_uint64_t                     event_count;
    bool                                     block_writes{false};
    bool                                     disable_frequent{false};

    virtual ~ProfileWriter();

    FORCEDINLINE void write(BytesConst&& data, bool blockable_write = false)
    {
        if(block_writes && blockable_write)
            return;
        if(auto error = platform::file::write(logfile, data.as<const char>());
           error.has_value())
            return;
    }
};

ProfileWriter::~ProfileWriter()
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    block_writes = true;

    auto thread_name = Coffee::Strings::fmt(
        R"({{"name":"process_name","ph":"M","pid":1,"args":{{"name":"{0}"}}}},)",
        appData ? appData->application_name : "Coffee App");
    write(BytesConst::ofContainer(thread_name));

    for(auto const& thread : stl_types::Threads::GetNames(threadState.get()))
    {
        thread_name = Coffee::Strings::fmt(
            R"({{"name":"thread_name","ph":"M","pid":1,"tid":{0},"args":{{"name":"{1}"}}}},)",
            thread.first,
            thread.second);
        write(BytesConst::ofContainer(thread_name));
    }

    for(auto const& metric : metrics::data)
    {
        auto out = Coffee::Strings::fmt(
            R"({{"name":"metric_name","ph":"M","id":{0},"args":{{"name":"{1}","type":{2}}}}},)",
            metric.second.id,
            metric.first,
            C_CAST<int>(metric.second.variant));
        write(BytesConst::ofContainer(out));
    }

    write(BytesConst::ofString("{}],"));

    std::string chromeInfo;
    Coffee::Profiling::ExportChromeTracerData(chromeInfo);

    if(auto idx = chromeInfo.find(R"(,"traceEvents")");
       idx != std::string::npos)
    {
        chromeInfo.resize(idx);
    }

    /* Okay, this is a stupid thing to do, but here goes:
     *  - Here we skip the opening { such that we may merge it with the
     * preceding object. This lets us plug it into the same model as before.
     *    This is also more compatible with the Chrome Trace Format.
     */
    write(*BytesConst::ofContainer(chromeInfo).at(1));
    write(BytesConst::ofString("}"));

    logfile = {};
}

std::shared_ptr<Coffee::State::GlobalState> CreateProfiler()
{
    if constexpr(!compile_info::profiler::enabled)
        return {};

    auto profile = url::constructors::MkUrl("profile.json", RSCA::TempFile);

    /* TODO: Truncate? */

    return std::make_shared<ProfileWriter>(profile);
}

static void platform_trace_begin(
    UNUSED_PARAM(profiling::DataPoint const&, point))
{
#if defined(COFFEE_ANDROID) && ANDROID_API_LEVEL > 23
    ATrace_beginSection(point.name.c_str());
#endif
}

static void platform_trace_end(UNUSED_PARAM(profiling::DataPoint const&, point))
{
#if defined(COFFEE_ANDROID) && ANDROID_API_LEVEL > 23
    ATrace_endSection();
#endif
}

void Push(profiling::ThreadState& tdata, profiling::DataPoint const& point)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    using namespace profiling;

    auto profileData = C_DCAST<ProfileWriter>(tdata.writer);

    if(!profileData || profileData->disable_frequent)
        return;

    const char* eventType   = "i";
    bool        is_async    = point.flags.attrs & DataPoint::Async;
    bool is_explicit_thread = point.flags.attrs & DataPoint::ExplicitThread;

    switch(point.flags.type)
    {
    case DataPoint::Push:
        if(!is_async && !is_explicit_thread)
            platform_trace_begin(point);
        eventType = is_async ? "b" : "B";
        break;
    case DataPoint::Pop:
        if(!is_async && !is_explicit_thread)
            platform_trace_end(point);
        eventType = is_async ? "e" : "E";
        break;
    default:
        break;
    }

    auto thread_name = is_explicit_thread
                           ? stl_types::Threads::GetName(point.tid)
                           : point.thread_name;

    if(thread_name.empty())
        thread_name = str::print::pointerify(point.tid);

    auto event = Coffee::Strings::fmt(
        event_format,
        std::chrono::duration_cast<std::chrono::microseconds>(point.ts).count(),
        point.name,
        point.tid,
        point.component,
        eventType);

    event = str::transform::printclean(event);

    profileData->write(BytesConst::ofContainer(event), true);
    profileData->event_count++;
}

extern void CaptureMetrics(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    std::string const&        value,
    std::chrono::microseconds ts,
    u32                       index)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto& profiler = *C_DCAST<ProfileWriter>(tdata.writer);

    if(profiler.disable_frequent)
        return;

    auto it = metrics::data.find(name);

    if(it == metrics::data.end())
    {
        it           = metrics::data.insert({name, {}}).first;
        auto& data   = it->second;
        data.id      = ++metrics::count;
        data.variant = variant;
    }

    auto const& data = it->second;

    constexpr auto metric_format =
        R"({{"ts":{2},"ph":"m","i":{3},"id":{0},"v":"{1}"}},
)";

    auto out = Coffee::Strings::fmt(
        metric_format, data.id, value, ts.count(), index);

    profiler.write(BytesConst::ofContainer(out));
}

void CaptureMetrics(
    profiling::ThreadState&   tdata,
    std::string_view          name,
    MetricVariant             variant,
    f32                       value,
    std::chrono::microseconds ts,
    u32                       index)
{
    using stl_types::cast_pod;
    CaptureMetrics(tdata, name, variant, cast_pod(value), ts, index);
}

} // namespace json
} // namespace profiling
} // namespace platform
