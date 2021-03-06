#include <coffee/core/logging/jsonlogger.h>

#include <coffee/core/coffee.h>
#include <coffee/core/internal_state.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/time_types.h>
#include <platforms/file.h>

/* to_string functions for formatting */
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/formatting.h>

#include <coffee/core/printing/outputprinter.h>

namespace Coffee {

using namespace ::platform::file;
using namespace ::semantic;
using namespace ::semantic::debug;
using namespace ::stl_types;

struct JsonLogState : State::GlobalState
{
    virtual ~JsonLogState();

    FileFun::FileHandle handle;
};

static const cstring JsonTaggedFormat =
    R"({
    "message": "{0}",
    "severity": "{1}",
    "pipe": "{2}",
    "level": "{3}",
    "tag": "{4}",
    "time": "{5}"
  },)";

static const cstring JsonFormat =
    R"({
    "message": "{0}",
    "severity": "{1}",
    "pipe": "{2}",
    "level": "{3}",
    "time": "{4}"
  },)";

static CString JsonFilter(CString const& message)
{
    CString filtered_message = message;
    filtered_message = str::replace::str(filtered_message, "\\", "\\\\");
    filtered_message = str::replace::str(filtered_message, "\n", "\\n");
    return str::replace::str(filtered_message, "\"", "\\\"");
}

static JsonLogState& GetLogState()
{
    return *C_DCAST<JsonLogState>(State::PeekState("jsonLog").get());
}

#if defined(COFFEE_WINDOWS)
#define fileno _fileno
#endif

static void JsonLogger(
    FILE* pipe, CString const& message, Severity sev, u32 level, u32)
{
    auto  filtered_message = JsonFilter(message);
    auto& jsonLog          = GetLogState().handle;

    auto json_msg = Strings::fmt(
        JsonFormat,
        filtered_message,
        Strings::to_string(sev),
        fileno(pipe),
        level,
        Time<>::Microsecond() / 1000);

    file_error ec;
    FileFun::Write(jsonLog, Bytes::CreateString(json_msg.c_str()), ec);
}

static void JsonTagLogger(
    FILE*          pipe,
    cstring        tag,
    CString const& message,
    Severity       sev,
    u32            level,
    u32)
{
    auto  filtered_message = JsonFilter(message);
    auto& jsonLog          = GetLogState().handle;

    auto json_msg = Strings::fmt(
        JsonTaggedFormat,
        filtered_message,
        Strings::to_string(sev),
        fileno(pipe),
        level,
        tag,
        Time<>::Microsecond() / 1000);

    FileFun::file_error ec;
    FileFun::Write(jsonLog, Bytes::CreateString(json_msg.c_str()), ec);
}

static void JsonLoggerExit()
{
    auto jsonLog = std::move(GetLogState().handle);

    file_error ec;
    FileFun::Write(jsonLog, Bytes::CreateString("{}\n]\n"), ec);

    DebugFun::SetLogInterface(
        {DebugFun::OutputPrinter::fprintf_platform,
         DebugFun::OutputPrinter::fprintf_platform_tagged});
}

JsonLogState::~JsonLogState()
{
}

DebugFun::LogInterface SetupJsonLogger(platform::url::Url const& jsonFilename)
{
    file_error ec;
    FileFun::Truncate(jsonFilename, 0, ec);

    auto jsonFile = FileFun::Open(
        jsonFilename,
        RSCA::ReadWrite | RSCA::Append | RSCA::Discard | RSCA::NewFile,
        ec);

    FileFun::Write(jsonFile, Bytes::CreateString("[\n  "), ec);

    auto jsonState    = MkShared<JsonLogState>();
    jsonState->handle = std::move(jsonFile);
    State::SwapState("jsonLog", jsonState);
    libc::signal::register_atexit(JsonLoggerExit);

    return {JsonLogger, JsonTagLogger};
}

} // namespace Coffee
