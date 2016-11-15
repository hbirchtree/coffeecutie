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
    const constexpr cstring var_snappy = "SNAP";

    if(fn[0] == '/')
        return fn;

    if(AssetApi::GetAsset(fn))
    {
        /* Check if it's an AppImage */
        if(Env::ExistsVar(var_appimg))
        {
            cstring asset = AssetApi::GetAsset(fn);
            CString res = Env::GetVar(var_appimg);
            CString out = Env::ConcatPath(res.c_str(),asset);
            return out;
        }
        /* Check if it's a Snap */
        else if(Env::ExistsVar(var_snappy))
        {
            cstring asset = AssetApi::GetAsset(fn);
            if(!Env::ExistsVar(var_snappy))
                return asset;
            CString res = Env::GetVar(var_snappy);
            res = Env::ConcatPath(res.c_str(),"assets");
            CString out = Env::ConcatPath(res.c_str(),asset);
            return out;
        }else{
            return AssetApi::GetAsset(fn);
        }
    }else
    {
        return FileFun::DereferenceLink(fn);
    }
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

CString LinuxFileFun::sys_read(cstring fn)
{
    CString out;
    FILE* fh = fopen(fn,"r");
    char* arg = 0;
    size_t size = 0;

    if(!fh)
        return out;

    while(getdelim(&arg,&size,0,fh) != -1)
    {
        out.append(arg);
    }
    free(arg);
    fclose(fh);
    if(out.size() > 0)
        out.resize(out.size()-1);
    return out;
}

}

}
}
