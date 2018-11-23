#include <platforms/linux/stacktrace.h>

#include <peripherals/libc/terminal.h>
#include <peripherals/stl/stlstring_ops.h>

#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_EXCEPTION_RETHROW)
#include <execinfo.h>
#endif

using namespace ::stl_types;

namespace platform {
namespace env {
namespace Linux {

STATICINLINE CString DemangleBacktrace(char* sym)
{
    CString sym_      = sym;
    auto    sym_end   = sym_.rfind('+');
    auto    sym_begin = sym_.rfind('(', sym_end);
    if(sym_end != CString::npos && sym_begin != CString::npos)
    {
        auto sym_length = sym_end - sym_begin - 1;
        auto sym_target = sym_.substr(sym_begin + 1, sym_length);

        sym_ = str::replace::str(
            sym_, sym_target, Stacktracer::DemangleSymbol(sym_target));
    }

    return sym_;
}

void Stacktracer::ExceptionStacktrace(const ExceptionPtr& exc_ptr)
{
#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_EXCEPTION_RETHROW) && 0
    static constexpr szptr MAX_CONTEXT = 20;

    void* tracestore[MAX_CONTEXT];

    try
    {
        if(exc_ptr)
            std::rethrow_exception(exc_ptr);
    } catch(std::exception& e)
    {
        if(libc::io::terminal::interactive())
            cBasicPrint(
                "{0}",
                str::transform::multiply(
                    '-', libc::io::terminal::size().first));
        cBasicPrint("exception encountered:");
        cBasicPrint(
            " >> {0}: {1}",
            Stacktracer::DemangleSymbol(typeid(e).name()),
            e.what());
        auto num  = backtrace(tracestore, MAX_CONTEXT);
        auto syms = backtrace_symbols(tracestore, num);
        if(syms && num)
        {
            cBasicPrint("dumping stacktrace:");
            for(auto i : Range<>(C_FCAST<szptr>(num)))
            {
                if(syms[i])
                {
                    cBasicPrint(" >> {0}", DemangleBacktrace(syms[i]));
                } else
                    cBasicPrint(
                        " >> {0}", Stacktracer::DemangleSymbol(syms[i]));
            }
        }
        free(syms);
    }
#endif
}

CString Stacktracer::GetFuncName_Internal(void* funcPtr)
{
    auto funcName = backtrace_symbols(&funcPtr, 1);

    if(!funcName)
        return {};

    CString out = DemangleBacktrace(funcName[0]);
    free(funcName);

    return out;
}

} // namespace Linux
} // namespace env
} // namespace platform
