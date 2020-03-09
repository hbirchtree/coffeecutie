#include <platforms/profiling.h>

#include <coffee/core/base_state.h>
#include <coffee/core/coffee.h>
#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/file.h>
#include <rapidjson/filewritestream.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/formatting.h>

namespace platform {
namespace profiling {

using namespace ::Coffee::DataStorage::TextStorage::RJSON;
using namespace ::platform::file;

static constexpr cstring event_format =
    R"({"ts":{0},"name":"{1}","pid":1,"tid":{2},"cat":"{3}","ph":"{4}","s":"t"},
)";

struct JsonProfileWriter : GlobalState
{
    JsonProfileWriter(Url outputProfile)
    {
        if constexpr(!build_props::profiler::enabled)
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

        C_ERROR_CHECK(ec);

        FileFun::Write(
            logfile,
            Bytes::CreateString(R"({"displayTimeUnit": "ms","traceEvents":[)"),
            ec);

        C_ERROR_CHECK(ec);
    }

    FileFun::FileHandle            logfile;
    ShPtr<GlobalState>             threadState;
    ShPtr<platform::info::AppData> appData;
    AtomicUInt64                   event_count;

    virtual ~JsonProfileWriter();
};

JsonProfileWriter::~JsonProfileWriter()
{
    if constexpr(!build_props::profiler::enabled)
        return;

    FileFun::file_error ec;

    auto thread_names = Coffee::Strings::fmt(
        R"({"name":"process_name","ph":"M","pid":1,"args":{"name":"{0}"}},)",
        appData ? appData->application_name : "Coffee App");

    for(auto const& thread : stl_types::Threads::GetNames(threadState.get()))
    {
        thread_names += Coffee::Strings::fmt(
            R"({"name":"thread_name","ph":"M","pid":1,"tid":{0},"args":{"name":"{1}"}},)",
            thread.first,
            thread.second);
    }

    FileFun::Write(logfile, Bytes::CreateString(thread_names.c_str()), ec);

    FileFun::Write(logfile, Bytes::CreateString(R"({}]})"), ec);

    FileFun::Close(std::move(logfile), ec);
}

ShPtr<Coffee::State::GlobalState> CreateJsonProfiler()
{
    if constexpr(!build_props::profiler::enabled)
        return {};

    auto profile = constructors::MkUrl("profile.json", RSCA::TempFile);

    FileFun::file_error ec;
    FileFun::Truncate(profile, 0, ec);

    C_ERROR_CHECK(ec);

    return MkShared<JsonProfileWriter>(profile);
}

void JsonPush(profiling::ThreadState& tdata, profiling::DataPoint const& point)
{
    if constexpr(!build_props::profiler::enabled)
        return;

    using namespace profiling;

    auto profileData = C_DCAST<JsonProfileWriter>(tdata.writer);

    if(!profileData)
        return;

    const char* eventType = "i";

    switch(point.flags.type)
    {
    case DataPoint::Push:
        eventType = point.flags.attrs & DataPoint::Async ? "b" : "B";
        break;
    case DataPoint::Pop:
        eventType = point.flags.attrs & DataPoint::Async ? "e" : "E";
        break;
    default:
        break;
    }

    auto thread_name = point.flags.attrs & DataPoint::ExplicitThread
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

    FileFun::file_error ec;
    FileFun::Write(
        profileData->logfile, Bytes::CreateString(event.c_str()), ec);

    C_ERROR_CHECK(ec);

    profileData->event_count++;
}

} // namespace profiling
} // namespace platform
