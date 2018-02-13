#include <coffee/core/base/debug/debugprinter.h>
#include <coffee/core/plat/plat_environment.h>

namespace Coffee{
namespace DebugFun{

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

void DebugPrinterImpl::AddContextString(CString& prefix, Severity sev)
{
    cstring severity_str = severity_string(sev);

    CString cclock = Time::ClockString();
#if defined(COFFEE_WINDOWS)
    /* VC++ fills the string with \0, and does not ignore it
     *  while appending. This is a big problem. */
    auto cclock_clip = cclock.find('\0');
    if(cclock_clip != CString::npos)
        cclock.resize(cclock_clip);
#endif

#if !defined(COFFEE_PLATFORM_OUTPUT_FORMAT)
    CString ms_time = cast_pod(Time::Microsecond() % 1000);
    CString clock = cStringFormat("{0}.{1}",
                                  cclock,
                                  StrUtil::lpad(ms_time, '0', 3)
                                  );
    prefix = cStringFormat("{0}:", clock.c_str());
    prefix.push_back(severity_str[0]);

    ColorMap::ColorText(prefix, ColorMap::CombineFormat(CmdColor::Green, CmdColor::Blue));
#endif
}

void OutputPrinterImpl::fprintf_platform(FILE *stream, CString formatted,
                                         Severity sev, bool locking)
{
    C_USED(sev);

    if(locking)
        PrinterLock.lock();
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

    __android_log_print(flag, "Coffee", "%s", &formatted[0]);
#elif defined(COFFEE_EMSCRIPTEN)
    int flag = 0;
    if(formatted[0] == 'W')
        flag = EM_LOG_WARN;
    else if(formatted[0] == 'F')
        flag = EM_LOG_ERROR;
    emscripten_log(flag, "%s", &formatted[0]);
#elif defined(COFFEE_WINDOWS_UWP)
    CWString formatted_w = StrUtil::convertformat<wbyte_t>(formatted);
    OutputDebugString(formatted_w.c_str());
#elif defined(COFFEE_WINDOWS)
    if(Env::GetVar("VisualStudioVersion").size())
        OutputDebugString(formatted.c_str());
    else
        Puts(stream, formatted.c_str());
#else
    Puts(stream,formatted.c_str());
#endif
    if(locking)
        PrinterLock.unlock();
}

}
}
