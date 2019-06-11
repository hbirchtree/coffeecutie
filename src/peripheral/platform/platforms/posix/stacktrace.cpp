#include <platforms/posix/stacktrace.h>

#if defined(COFFEE_UNIXPLAT)

#include <peripherals/base.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/ptr_wrap.h>
#include <peripherals/stl/regex.h>
#include <peripherals/stl/string_ops.h>

#include <cxxabi.h> //Demangling function names

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h> //For retrieving the callstack
#endif

#if defined(COFFEE_GLIBC_STACKTRACE)
#include <execinfo.h>
#endif

namespace platform {
namespace env {
namespace posix {

using namespace ::stl_types;
using namespace ::semantic;

CString Stacktracer::DemangleSymbol(const char* sym)
{
#ifndef COFFEE_LOWFAT
    i32 stat = 0;

    Ptr<char, ptr_opts::managed_ptr> symbol =
        abi::__cxa_demangle(sym, nullptr, nullptr, &stat);

    if(stat == 0)
    {
        CString outSymbol = symbol.ptr;
        return outSymbol;
    } else
#endif
        return sym;
}

StacktracerDef::Stacktrace Stacktracer::GetRawStackframes(
    C_UNUSED(u32 start), C_UNUSED(i32 length))
{
    Stacktrace t;
#ifndef COFFEE_LOWFAT
#if defined(COFFEE_USE_UNWIND)
    if(!unwind_context)
    {
        unwind_context = new unw_context_t;
        unw_getcontext(unwind_context);
    }

    unw_cursor_t cursor;
    unw_init_local(&cursor, unwind_context);

    for(uint32 i = 0; i < start; i++)
        unw_step(&cursor);

    CString temp_buf;
    temp_buf.resize(256);

    int32 depth = 0;

    while(unw_step(&cursor) > 0)
    {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if(pc == 0)
            break;
        if(unw_get_proc_name(
               &cursor,
               &temp_buf[0],
               temp_buf.size() * sizeof(char),
               &offset) == 0)
        {
            CString fname = DemangleSymbol(temp_buf);
            t.push_back(fname);
        }

        depth++;
        if(length != (-1) && depth == length)
            break;
    }
#endif
#endif
    return t;
}

CString Stacktracer::GetStackframeName(u32 depth)
{
#ifndef COFFEE_LOWFAT
    Stacktrace trace = GetRawStackframes(depth, 2);
    if(!trace.size())
        return "???";
    return CString(trace[0]);
#else
    return {};
#endif
}

CString Stacktracer::GetStackFuncName(u32 depth)
{
#ifndef COFFEE_LOWFAT
    static bool           rgx_compiled;
    static regex::Pattern rgx;

    CString frame = GetStackframeName(depth + 1);

    if(!rgx_compiled)
    {
        rgx = regex::compile_pattern("^(.*)\\(.*$");
    }

    Vector<CString> result;

    if(!regex::match(rgx, frame, result))
        return frame;

    return result[1];
#else
    return {};
#endif
}

} // namespace posix


#if defined(COFFEE_GLIBC_STACKTRACE)
namespace glibc {

STATICINLINE CString DemangleBacktrace(char* sym)
{
    CString sym_      = sym;
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    /* glibc's format */
    auto    sym_end   = sym_.rfind('+');

#if defined(COFFEE_APPLE)
    /* macOS format looks like this:
     * 0   GLeamBaseTest_RHI                   0x000000010030c86a SIGNATURE + 0
     *                                  What we want    ~~~~~~~~~^
     * We fix this by adjusting the beginning index
     */
    auto sym_begin = sym_.rfind(' ', sym_end);
    sym_begin = sym_.rfind(' ', sym_begin - 1);

    sym_end -= 1;
#else
    auto    sym_begin = sym_.rfind('(', sym_end);
#endif

    if(sym_end != CString::npos && sym_begin != CString::npos)
    {
        auto sym_length = sym_end - sym_begin - 1;
        auto sym_target = sym_.substr(sym_begin + 1, sym_length);

        sym_ = str::replace::str(
            sym_, sym_target, Stacktracer::DemangleSymbol(sym_target));
    }
#endif

    return sym_;
}

static void DefaultedPrint(
    typing::logging::LogInterfaceBasic logger, CString const& line)
{
    using semantic::debug::Severity;
    using namespace libc::io;

    logger(io_handles::err, line, Severity::Critical, 1, 0);
}

void Stacktracer::ExceptionStacktrace(
    const ExceptionPtr& exc_ptr, typing::logging::LogInterfaceBasic log)
{
    static constexpr szptr MAX_CONTEXT = 20;

    void* tracestore[MAX_CONTEXT];

    try
    {
        if(exc_ptr)
            std::rethrow_exception(exc_ptr);
    } catch(std::exception& e)
    {
        if(libc::io::terminal::interactive())
            DefaultedPrint(
                log,
                str::transform::multiply(
                    '-', libc::io::terminal::size().first));
        DefaultedPrint(log, "exception encountered:");
        DefaultedPrint(
            log,
            " >> " + Stacktracer::DemangleSymbol(typeid(e).name()) + ": " +
                e.what());
        auto num  = backtrace(tracestore, MAX_CONTEXT);
        auto syms = backtrace_symbols(tracestore, num);
        if(syms && num)
        {
            DefaultedPrint(log, "dumping stacktrace:");
            for(auto i : Range<>(C_FCAST<szptr>(num)))
            {
                if(syms[i])
                {
                    DefaultedPrint(log, " >> " + DemangleBacktrace(syms[i]));
                } else
                    DefaultedPrint(
                        log, " >> " + Stacktracer::DemangleSymbol(syms[i]));
            }
        }
        free(syms);
    }
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

} // namespace glibc
#endif

} // namespace env
} // namespace platform

#endif
