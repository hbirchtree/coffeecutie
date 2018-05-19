#pragma once

#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{

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
    static CString GetStackframeName(uint32 = 0)
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
     * \brief Dump the full stack, alternatively from a start (first argument) to given length (second argument)
     * \return A vector containing stackframe names
     */
    static Stacktrace GetFullStack(uint32 = 0, int32 = -1)
    {
        return {};
    }
};

}
