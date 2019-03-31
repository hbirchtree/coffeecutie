#include <platforms/profiling.h>

#include <coffee/core/coffee.h>
#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/file.h>
#include <rapidjson/filewritestream.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/formatting.h>

namespace platform {
namespace profiling {

using namespace DataStorage::TextStorage::RJSON;
using namespace ::platform::file;

#if !defined(COFFEE_DISABLE_PROFILER)
static constexpr cstring event_format =
    R"({"ts":{0},"name":"{1}","pid":1,"tid":{2},"cat":"{3}","ph":"{4}","s":"t"},
)";

struct JsonProfileWriter : State::GlobalState
{
    JsonProfileWriter(Url outputProfile)
    {
        /* We keep a reference to this pointer in order to extend its lifespan.
         * In the destructor we need it for finalizing the thread names. */
        threadState = State::PeekState("threadNames");
        appData = State::GetAppData();
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
    ShPtr<State::GlobalState>      threadState;
    ShPtr<platform::info::AppData> appData;
    AtomicUInt64                   event_count;

    virtual ~JsonProfileWriter();
};

JsonProfileWriter::~JsonProfileWriter()
{
    FileFun::file_error ec;

    auto thread_names = Strings::fmt(
        R"({"name":"process_name","ph":"M","pid":1,"args":{"name":"{0}"}},)",
        appData ? appData->application_name : "Coffee App");

    for(auto const& thread : stl_types::Threads::GetNames(threadState.get()))
    {
        thread_names += Strings::fmt(
            R"({"name":"thread_name","ph":"M","pid":1,"tid":{0},"args":{"name":"{1}"}},)",
            thread.first,
            thread.second);
    }

    FileFun::Write(logfile, Bytes::CreateString(thread_names.c_str()), ec);

    FileFun::Write(logfile, Bytes::CreateString(R"({}]})"), ec);

    FileFun::Close(std::move(logfile), ec);
}
#endif

ShPtr<State::GlobalState> CreateJsonProfiler()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    auto profile = constructors::MkUrl("profile.json", RSCA::TempFile);

    FileFun::file_error ec;
    FileFun::Truncate(profile, 0, ec);

    C_ERROR_CHECK(ec);

    return MkShared<JsonProfileWriter>(profile);
#else
    return {};
#endif
}

void JsonPush(profiling::ThreadState& tdata, profiling::DataPoint const& point)
{
    using namespace profiling;

#if !defined(COFFEE_DISABLE_PROFILER)
    auto profileData = C_DCAST<JsonProfileWriter>(tdata.writer);

    if(!profileData)
        return;

    const char* eventType = "i";

    switch(point.flags.type)
    {
    case DataPoint::Push:
        eventType = "B";
        break;
    case DataPoint::Pop:
        eventType = "E";
        break;
    default:
        break;
    }

    auto thread_name = ThreadGetName(point.tid);

    if(!thread_name.size())
        thread_name = str::print::pointerify(point.tid);

    auto event = Strings::fmt(
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
#endif
}

} // namespace profiling
} // namespace platform
