#include <coffee/core/internal_state.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/formatting.h>

#include <coffee/core/printing/outputprinter.h>

#if defined(COFFEE_ANDROID)
#include <android/log.h>
#endif

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Coffee {
namespace DebugFun {

cstring severity_string(Severity sev)
{
    cstring const severity_info = "INFO";
    cstring const severity_debg = "DEBG";
    cstring const severity_warn = "WARN";
    cstring const severity_ftal = "FTAL";
    cstring const severity_verb = "VERB";

    cstring severity_str = nullptr;

    switch(sev)
    {
    case Severity::Information:
        severity_str = severity_info;
        break;
    case Severity::Verbose:
        severity_str = severity_verb;
        break;
    case Severity::Debug:
    case Severity::Low:
        severity_str = severity_debg;
        break;
    case Severity::Medium:
        severity_str = severity_warn;
        break;
    case Severity::Critical:
    case Severity::Fatal:
    case Severity::High:
        severity_str = severity_ftal;
        break;
    }

    return severity_str;
}

static void AddContextString(CString& prefix, Severity sev)
{
    cstring severity_str = severity_string(sev);

    CString cclock = TimeFormatter<>::String("%H:%M:%S");

    if constexpr(compile_info::platform::is_windows)
    {
        /* VC++ fills the string with \0, and does not ignore it
         *  while appending. This is a big problem. */
        auto cclock_clip = cclock.find('\0');
        if(cclock_clip != CString::npos)
            cclock.resize(cclock_clip);
    }

    if constexpr(!compile_info::printing::is_syslogged)
    {
        CString ms_time = cast_pod((Time<>::Microsecond() / 1000) % 1000);
        CString clock   = Strings::cStringFormat(
            "{0}.{1}", cclock, str::pad::left(ms_time, '0', 3));
        prefix = Strings::cStringFormat("{0}:", clock.c_str());
        prefix.append(CurrentThread::GetName() + ":");
        prefix.push_back(severity_str[0]);

        platform::ColorMap::ColorText(
            prefix,
            platform::ColorMap::CombineFormat(
                platform::ColorMap::CmdColor::Green,
                platform::ColorMap::CmdColor::Blue));
    }
}

static void native_print(
    FILE* stream, CString const& formatted, C_UNUSED(Severity sev))
{
    /* If printing to file, don't lock IO */
    bool locking       = (stream == stdout || stream == stderr);
    bool locking_state = C_OCAST<bool>(State::GetInternalState());

    if constexpr(!compile_info::lowfat_mode)
        if(locking && locking_state)
            State::GetPrinterLock().lock();

#if defined(COFFEE_ANDROID)
    int flag = 0;

    switch(sev)
    {
    case Severity::Information:
        flag = ANDROID_LOG_INFO;
        break;
    case Severity::Debug:
        flag = ANDROID_LOG_DEBUG;
        break;
    case Severity::Medium:
        flag = ANDROID_LOG_WARN;
        break;
    case Severity::Fatal:
        flag = ANDROID_LOG_ERROR;
        break;
    case Severity::Verbose:
        flag = ANDROID_LOG_VERBOSE;
        break;
    default:
        break;
    }

    __android_log_print(flag, "Coffee", "%s", formatted.c_str());
#elif defined(COFFEE_EMSCRIPTEN)
    int flag = 0;
    if(formatted[0] == 'W')
        flag = EM_LOG_WARN;
    else if(formatted[0] == 'F')
        flag = EM_LOG_ERROR;
    emscripten_log(flag, "%s", formatted.c_str());
#elif defined(COFFEE_WINDOWS)
    auto formatted_w = str::encode::to<wchar_t>(formatted);
    if(platform::Env::ExistsVar("VisualStudioVersion"))
        OutputDebugStringW(formatted_w.c_str());
    else
        libc::io::put(stream, formatted.c_str());
#elif defined(COFFEE_IOS)
    libc::io::put(libc::io::io_handles::out, formatted.c_str());
#else
    libc::io::put(stream, formatted.c_str());
#endif

    if constexpr(!compile_info::lowfat_mode)
        if(locking && locking_state)
            State::GetPrinterLock().unlock();
}

void OutputPrinterImpl::fprintf_platform_tagged(
    FILE*          stream,
    cstring        tag,
    const CString& formatted_raw,
    Severity       sev,
    u32            level,
    u32            flags)
{
    if(PrintingVerbosityLevel() < level)
        return;

    if constexpr(!compile_info::printing::is_syslogged)
    {
        CString formatted;

        if constexpr(!compile_info::printing::is_simple)
        {
            if(!(flags & Flag_NoContext))
            {
                AddContextString(formatted, sev);

                if(tag)
                    (formatted += ":") += tag;

                (formatted += ": ") += formatted_raw;
            } else
                formatted = formatted_raw;
        } else
            formatted = formatted_raw;

        if(!(flags & Flag_NoNewline))
            formatted += "\n";

        native_print(stream, formatted, sev);

    } else
        native_print(stream, formatted_raw, sev);
}

} // namespace DebugFun
} // namespace Coffee
