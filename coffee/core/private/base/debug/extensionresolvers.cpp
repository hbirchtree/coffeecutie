#include <coffee/core/base/debug/extensionresolvers.h>

#include <coffee/core/plat/memory/stlstring_ops.h>

namespace Coffee{
namespace DebugFun{

using namespace Mem;
using namespace Mem::Search;

CString cStringReplace(const CString &fmt, const size_t &index, const scalar &arg)
{
    return cStringReplace(fmt,index,C_CAST<bigscalar>(arg));
}

CString cStringReplace(const CString &fmt, const size_t &index, const CString &arg)
{
    return extArgReplace(fmt,index,arg);
}

CString cStringReplace(const CString &fmt, const size_t &index, const bool &arg)
{
    return extArgReplace(fmt,index,(arg) ? "true" : "false");
}

CString cStringReplace(const CString &fmt, const size_t &index, char * const arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

CString cStringReplace(const CString &fmt, const size_t &index, cstring arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

CString extArgReplacePhrase(const CString &fmt, const CString &phrase, const CString &replace)
{
    return CStrReplace(fmt,phrase,replace);
}

CString extArgReplace(const CString &fmt, const size_t &index, const CString &replace)
{
    CString subfmt = "{" + Convert::uinttostring(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
}

}
}
