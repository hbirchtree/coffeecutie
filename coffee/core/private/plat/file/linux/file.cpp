#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/plat_environment.h>

#include "../file_abstraction.h"
#include <coffee/core/coffee_version.h>

namespace Coffee{
namespace CResources{
namespace Linux{

CString LinuxFileFun::NativePath(cstring fn)
{
    const constexpr cstring var_appimg = "APPIMAGE_DATA_DIR";
    const constexpr cstring var_snappy = "SNAP_DATA";

    if(fn[0] == '/')
        return fn;

    if(AssetApi::GetAsset(fn))
    {
        /* Check if it's an AppImage */
        {
            if(!Env::ExistsVar(var_appimg))
                return AssetApi::GetAsset(fn);
            cstring asset = AssetApi::GetAsset(fn);
            CString res = Env::GetVar(var_appimg);
            CString out = Env::ConcatPath(res.c_str(),asset);
            return out;
        }
        /* Check if it's a Snap */
        {
            cstring asset = AssetApi::GetAsset(fn);
            if(!Env::ExistsVar(var_snappy))
                return asset;
            CString res = Env::GetVar(var_snappy);
            CString out = Env::ConcatPath(res.c_str(),asset);
            return out;
        }
    }else
        return fn;
}

CString LinuxFileFun::NativePath(cstring fn, ResourceAccess storage)
{
    if(feval(storage,ResourceAccess::ConfigFile))
    {
        CString udata = Env::GetUserData(CoffeeApplicationData::organization_name.c_str(),
                                         CoffeeApplicationData::application_name.c_str());
        return Env::ConcatPath(udata.c_str(),fn);
    }else if(feval(storage,ResourceAccess::TemporaryFile))
        return Env::ConcatPath("/tmp",fn);
    else
        return NativePath(fn);
}

bool LinuxFileFun::VerifyAsset(cstring fn)
{
    CString native = NativePath(fn);
    return LinuxFileFun::Exists(native.c_str());
}

}

}
}
