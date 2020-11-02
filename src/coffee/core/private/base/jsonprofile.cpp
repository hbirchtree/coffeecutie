#include <platforms/profiling/jsonprofile.h>

#include <coffee/core/base_state.h>
#include <coffee/core/coffee.h>
#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/file.h>
#include <rapidjson/filewritestream.h>

#include <coffee/core/profiler/profiling-export.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/formatting.h>

#if defined(COFFEE_ANDROID)
#include <android/trace.h>
#endif

namespace platform {
namespace profiling {
namespace json {

using namespace ::Coffee::DataStorage::TextStorage::RJSON;
using namespace ::platform::file;

static constexpr cstring event_format =
    R"({"ts":{0},"name":"{1}","pid":1,"tid":{2},"cat":"{3}","ph":"{4}","s":"t"},
)";

struct MetricData
{
    MetricVariant variant;
    u32           id;
};

namespace metrics {
static Map<cstring, MetricData> data;
static u32                      count;
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

        FileFun::file_error ec;
        logfile = FileFun::Open(
            outputProfile,
            RSCA::Append | RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile,
            ec);

        C_ERROR_CHECK(ec)

        FileFun::Write(
            logfile,
            Bytes::CreateString(R"({"displayTimeUnit": "ms","traceEvents":[)"),
            ec);

        C_ERROR_CHECK(ec)
    }

    FileFun::FileHandle            logfile;
    ShPtr<GlobalState>             threadState;
    ShPtr<platform::info::AppData> appData;
    AtomicUInt64                   event_count;

    virtual ~ProfileWriter();

    FORCEDINLINE void write(Bytes&& data)
    {
        file_error ec;
        FileFun::Write(logfile, data, ec);

        C_ERROR_CHECK(ec)
    }
};

ProfileWriter::~ProfileWriter()
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto thread_name = Coffee::Strings::fmt(
        R"({"name":"process_name","ph":"M","pid":1,"args":{"name":"{0}"}},)",
        appData ? appData->application_name : "Coffee App");
    write(Bytes::CreateString(thread_name.c_str()));

    for(auto const& thread : stl_types::Threads::GetNames(threadState.get()))
    {
        thread_name = Coffee::Strings::fmt(
            R"({"name":"thread_name","ph":"M","pid":1,"tid":{0},"args":{"name":"{1}"}},)",
            thread.first,
            thread.second);
        write(Bytes::CreateString(thread_name.c_str()));
    }

    for(auto const& metric : metrics::data)
    {
        auto out = Coffee::Strings::fmt(
            R"({"name":"metric_name","ph":"M","id":{0},"args":{"name":"{1}","type":{2}}},)",
            metric.second.id,
            metric.first,
            C_CAST<int>(metric.second.variant));
        write(Bytes::CreateString(out.c_str()));
    }

    write(Bytes::CreateString(R"({}],)"));

    CString chromeInfo;
    Coffee::Profiling::ExportChromeTracerData(chromeInfo);

    if(auto idx = chromeInfo.find(R"(,"traceEvents")"); idx != CString::npos)
    {
        chromeInfo.resize(idx);
    }

    /* Okay, this is a stupid thing to do, but here goes:
     *  - Here we skip the opening { such that we may merge it with the
     * preceding object. This lets us plug it into the same model as before.
     *    This is also more compatible with the Chrome Trace Format.
     */
    write(Bytes::CreateString(chromeInfo.c_str() + 1));

    write(Bytes::CreateString("}"));

    FileFun::file_error ec;
    FileFun::Close(std::move(logfile), ec);
    C_ERROR_CHECK(ec)
}

ShPtr<Coffee::State::GlobalState> CreateProfiler()
{
    if constexpr(!compile_info::profiler::enabled)
        return {};

    auto profile = constructors::MkUrl("profile.json", RSCA::TempFile);

    FileFun::file_error ec;
    FileFun::Truncate(profile, 0, ec);

    C_ERROR_CHECK(ec)

    return MkShared<ProfileWriter>(profile);
}

static void platform_trace_begin(
    UNUSED_PARAM(profiling::DataPoint const&, point))
{
#if defined(COFFEE_ANDROID)
    if constexpr(compile_info::android::api >= 23)
        ATrace_beginSection(point.name.c_str());
#endif
}

static void platform_trace_end(
    UNUSED_PARAM(profiling::DataPoint const&, point))
{
#if defined(COFFEE_ANDROID)
    if constexpr(compile_info::android::api >= 23)
        ATrace_endSection();
#endif
}

void Push(profiling::ThreadState& tdata, profiling::DataPoint const& point)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    using namespace profiling;

    auto profileData = C_DCAST<ProfileWriter>(tdata.writer);

    if(!profileData)
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
                           ? ThreadGetName(point.tid)
                           : point.thread_name;

    if(thread_name.empty())
        thread_name = str::print::pointerify(point.tid);

    auto event = Coffee::Strings::fmt(
        event_format,
        Chrono::duration_cast<Chrono::microseconds>(point.ts).count(),
        point.name,
        point.tid,
        point.component,
        eventType);

    event = str::transform::printclean(event);

    profileData->write(Bytes::CreateString(event.c_str()));
    profileData->event_count++;
}

extern void CaptureMetrics(
    profiling::ThreadState& tdata,
    cstring                 name,
    MetricVariant           variant,
    CString const&          value,
    Chrono::microseconds    ts,
    u32                     index)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

    auto it = metrics::data.find(name);

    if(it == metrics::data.end())
    {
        it           = metrics::data.insert({name, {}}).first;
        auto& data   = it->second;
        data.id      = ++metrics::count;
        data.variant = variant;
    }

        auto const& data     = it->second;
    auto&       profiler = *C_DCAST<ProfileWriter>(tdata.writer);

        constexpr auto metric_format =
            R"({"ts":{2},"ph":"m","i":{3},"id":{0},"v":"{1}"},
)";

    using namespace Chrono;

    auto out =
        Coffee::Strings::fmt(metric_format, data.id, value, ts.count(), index);

    profiler.write(Bytes::CreateString(out.c_str()));
}

void CaptureMetrics(
    profiling::ThreadState& tdata,
    cstring                 name,
    MetricVariant           variant,
    f32                     value,
    Chrono::microseconds    ts,
    u32                     index)
{
    CaptureMetrics(tdata, name, variant, cast_pod(value), ts, index);
}

} // namespace json
} // namespace profiling
} // namespace platform
