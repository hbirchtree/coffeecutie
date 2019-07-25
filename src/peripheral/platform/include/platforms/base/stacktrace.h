#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <peripherals/typing/logging.h>

namespace platform {
namespace env {

using namespace ::libc_types;
using namespace ::stl_types;

struct StacktracerDef
{
    using Stacktrace = Vector<CString>;

    template<typename T>
    /*!
     * \brief Get name of type
     * \return
     */
    static CString GetTypeName()
    {
#ifndef COFFEE_LOWFAT
        return typeid(T).name();
#else
        return {};
#endif
    }

    template<typename T>
    static CString GetFuncName(T /*funcPtr*/)
    {
        return {};
    }

    /*!
     * \brief Demangle C++ symbol
     * \param in
     * \return
     */
    static CString DemangleSymbol(CString const& in)
    {
        return in;
    }

    template<typename T>
    static CString DemType()
    {
        return DemangleSymbol(GetTypeName<T>());
    }

    /*!
     * \brief Will return stackframe name for calling function
     *  at depth 0 by default
     *  Argument specifies depth.
     * \return A string with the name of the stackframe
     */
    static CString GetStackframeName(u32 = 0)
    {
        return {};
    }

    /*!
     * \brief Same as GetStackframeName, but only gets function name
     * \return
     */
    static CString GetStackFuncName(u32 = 0)
    {
        return {};
    }

    /*!
     * \brief Dump the full stack, alternatively from a start (first argument)
     * to given length (second argument) \return A vector containing stackframe
     * names
     */
    static Stacktrace GetFullStack(u32 = 0, i32 = -1)
    {
        return {};
    }

    static void ExceptionStacktrace(ExceptionPtr const& exc,
                                    typing::logging::LogInterfaceBasic)
    {
        try
        {
            std::rethrow_exception(exc);
        } catch(std::exception const& e)
        {
            printf("Caught exception: %s: %s\n", typeid(e).name(), e.what());
        }
    }
};

#if defined(COFFEE_EMSCRIPTEN)
using Stacktracer = StacktracerDef;
#endif

} // namespace env
} // namespace platform
