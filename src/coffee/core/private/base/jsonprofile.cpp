#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/plat/timing/profiling.h>
#include <rapidjson/filewritestream.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Profiling {

using namespace DataStorage::TextStorage::RJSON;

static constexpr cstring event_format =
    R"({"ts":{0},"name":"{1}","pid":1,"tid":"{2}","cat":"{3}","ph":"{4}","s":"t"},)";

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

        FileFun::Write(logfile, Bytes::CreateString(R"({"displayTimeUnit": "ms","traceEvents":[)"), ec);

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

ShPtr<State::GlobalState> CreateJsonProfiler()
{
    auto profile = MkUrl("profile.json", RSCA::TempFile);

    FileFun::file_error ec;
    FileFun::Truncate(profile, 0, ec);

    return MkShared<JsonProfileWriter>(profile);
}

void JsonPush(ThreadData* tdata, DataPoint const& point)
{
    auto profileData = C_DCAST<JsonProfileWriter>(tdata->datapoints.writer);

    if(!profileData)
        return;

    auto eventType = "i";

    switch(point.tp)
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

    auto event =
        fmt(event_format,
            point.ts,
            point.name,
            StrUtil::pointerify(point.thread),
            point.component,
            eventType);

    event = StrUtil::printclean(event);

    FileFun::file_error ec;
    FileFun::Write(
        profileData->logfile, Bytes::CreateString(event.c_str()), ec);

    C_ERROR_CHECK(ec);

    profileData->event_count++;
}

} // namespace Profiling
} // namespace Coffee
