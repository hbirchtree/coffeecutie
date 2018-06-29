#include <coffee/core/plat/environment/linux/stacktrace.h>

#include <coffee/core/CMD>
#include <coffee/core/CDebug>

#if defined(COFFEE_LINUX)
#include <execinfo.h>
#endif

namespace Coffee{
namespace Environment{
namespace Linux {

void LinuxStacktracer::ExceptionStacktrace(const ExceptionPtr &exc_ptr)
{
#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_EXCEPTION_RETHROW)
    static constexpr szptr MAX_CONTEXT = 20;

    void* tracestore[MAX_CONTEXT];

    try {
        if(exc_ptr)
            std::rethrow_exception(exc_ptr);
    } catch(std::exception& e)
    {
        if(Cmd::Interactive())
            cBasicPrint("{0}", StrUtil::multiply('-', Cmd::TerminalSize().w));
        cBasicPrint("exception encountered:");
        cBasicPrint(" >> {0}: {1}",
                 Stacktracer::DemangleSymbol(typeid(e).name()),
                 e.what());
        auto num = backtrace(tracestore, MAX_CONTEXT);
        auto syms = backtrace_symbols(tracestore, num);
        if(syms && num)
        {
            cBasicPrint("dumping stacktrace:");
            for(auto i : Range<>(C_FCAST<szptr>(num)))
            {
                if(syms[i])
                {
                    CString sym_ = syms[i];
                    auto sym_end = sym_.rfind('+');
                    auto sym_begin = sym_.rfind('(', sym_end);
                    if(sym_end != CString::npos && sym_begin != CString::npos)
                    {
                        auto sym_length = sym_end - sym_begin - 1;
                        auto sym_target = sym_.substr(sym_begin + 1, sym_length);
                        sym_ = CStrReplace(
                                 sym_,
                                 sym_target,
                                 Stacktracer::DemangleSymbol(sym_target));
                        cBasicPrint(" >> {0}", sym_);
                    }
                }else
                    cBasicPrint(" >> {0}", Stacktracer::DemangleSymbol(syms[i]));
            }
        }
    }
#endif
}

}
}
}
