#include <coffee/core/base/debug/debugprinter.h>

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

}
}
