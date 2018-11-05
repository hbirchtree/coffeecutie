#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <coffee/core/plat/file.h>
#include <coffee/core/plat/timing/profiling.h>
#include <rapidjson/filewritestream.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Profiling {

using namespace DataStorage::TextStorage::RJSON;

#if !defined(COFFEE_DISABLE_PROFILER)
static constexpr cstring event_format =
    R"({"ts":{0},"name":"{1}","pid":1,"tid":"{2}","cat":"{3}","ph":"{4}","s":"t"},
)";

struct JsonProfileWriter : State::GlobalState
{
    JsonProfileWriter(Url outputProfile)
    {
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

    FileFun::FileHandle logfile;

    AtomicUInt64 event_count;

    virtual ~JsonProfileWriter();
};

JsonProfileWriter::~JsonProfileWriter()
{
    FileFun::file_error ec;

    FileFun::Write(logfile, Bytes::CreateString(R"({}]})"), ec);

    FileFun::Close(std::move(logfile), ec);
}
#endif

ShPtr<State::GlobalState> CreateJsonProfiler()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    auto profile = MkUrl("profile.json", RSCA::TempFile);

    FileFun::file_error ec;
    FileFun::Truncate(profile, 0, ec);

    C_ERROR_CHECK(ec);

    return MkShared<JsonProfileWriter>(profile);
#else
    return {};
#endif
}

void JsonPush(ThreadState& tdata, DataPoint const& point)
{
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

    auto event =
        fmt(event_format,
            Chrono::duration_cast<Chrono::microseconds>(point.ts).count(),
            point.name,
            ThreadGetName(point.tid),
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

} // namespace Profiling
} // namespace Coffee
