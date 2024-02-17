#include <coffee/core/logging/jsonlogger.h>

#include <coffee/core/coffee.h>
#include <coffee/core/debug/logging.h>
#include <coffee/core/internal_state.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/string/replace.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/time_types.h>
#include <platforms/file.h>

#include <fmt_extensions/info.h>

#include <coffee/core/printing/outputprinter.h>
#include <coffee/strings/format.h>

namespace Coffee {

using namespace ::platform::file;
using namespace ::semantic;
using namespace ::semantic::debug;
using namespace ::stl_types;

struct JsonLogState : State::GlobalState
{
    virtual ~JsonLogState();

    platform::file::file_handle handle;
};

// static const cstring JsonFormat =
//     R"({
//     "message": "{0}",
//     "severity": "{1}",
//     "pipe": "{2}",
//     "level": "{3}",
//     "time": "{4}"
//   },)";

static std::string JsonFilter(std::string_view message)
{
    std::string filtered_message(message.begin(), message.end());
    filtered_message = str::replace::str<char>(filtered_message, "\\", "\\\\");
    filtered_message = str::replace::str<char>(filtered_message, "\n", "\\n");
    return str::replace::str<char>(filtered_message, "\"", "\\\"");
}

static JsonLogState& GetLogState()
{
    return *C_DCAST<JsonLogState>(State::PeekState("jsonLog").get());
}

#if defined(COFFEE_WINDOWS)
#define fileno _fileno
#endif

static void JsonTagLogger(
    FILE*            pipe,
    std::string_view tag,
    std::string_view message,
    Severity         sev,
    u32              level,
    u32)
{
    auto  filtered_message = JsonFilter(message);
    auto& jsonLog          = GetLogState().handle;

    auto json_msg = Strings::fmt(
        R"({{
          "message": "{0}",
          "severity": "{1}",
          "pipe": "{2}",
          "level": "{3}",
          "tag": "{4}",
          "time": "{5}"
        }},)",
        filtered_message,
        sev,
        fileno(pipe),
        level,
        tag,
        Time<>::Microsecond() / 1000);

    platform::file::write(
        jsonLog, mem_chunk<const char>::ofContainer(json_msg));
}

static void JsonLoggerExit()
{
    auto jsonLog = std::move(GetLogState().handle);

    platform::file::write(jsonLog, mem_chunk<const char>::ofString("{}\n]\n"));

    DebugFun::SetLogInterface({Logging::log});
}

JsonLogState::~JsonLogState()
{
}

DebugFun::LogInterface SetupJsonLogger(platform::url::Url const& jsonFilename)
{
    //    FileFun::Truncate(jsonFilename, 0, ec);

    auto jsonFile = platform::file::open_file(
        jsonFilename,
        RSCA::ReadWrite | RSCA::Append | RSCA::Discard | RSCA::NewFile);

    if(jsonFile.has_error())
        return {};

    platform::file::write(
        jsonFile.value(), mem_chunk<const char>::ofString("[\n  "));

    auto jsonState    = std::make_shared<JsonLogState>();
    jsonState->handle = std::move(jsonFile.value());
    State::SwapState("jsonLog", jsonState);
    ::libc::signal::register_atexit(JsonLoggerExit);

    return {JsonTagLogger};
}

} // namespace Coffee
