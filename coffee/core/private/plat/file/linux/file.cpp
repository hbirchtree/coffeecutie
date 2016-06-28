#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/plat_environment.h>

#include "../file_abstraction.h"

namespace Coffee{
namespace CResources{
namespace Linux{

CString LinuxFileFun::NativePath(cstring fn)
{
    const constexpr cstring var_appimg = "APPIMAGE_DATA_DIR";

    if(AssetApi::GetAsset(fn))
    {
        if(!Env::ExistsVar(var_appimg))
            return AssetApi::GetAsset(fn);
        cstring asset = AssetApi::GetAsset(fn);
        CString res = Env::GetVar(var_appimg);
        CString out = Env::ConcatPath(res.c_str(),asset);
        return out;
    }else
        return fn;
}

bool LinuxFileFun::VerifyAsset(cstring fn)
{
    CString native = NativePath(fn);
    return LinuxFileFun::Exists(native.c_str());
}

}
}
}
