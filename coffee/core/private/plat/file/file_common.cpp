#include <coffee/core/plat/file/file_def.h>

#include <coffee/core/plat/plat_environment.h>

namespace Coffee{
namespace CResources{

CString DirFunDef::Basename(cstring fn)
{
    return Env::BaseName(fn);
}

}
}
