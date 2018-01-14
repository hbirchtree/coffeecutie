#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/plat_environment.h>

#include "../file_abstraction.h"
#include <coffee/core/coffee_version.h>
#include <coffee/core/coffee.h>

#include <stdio.h>

namespace Coffee{
namespace CResources{
namespace Linux{

#if !defined(COFFEE_ANDROID)
//CString LinuxFileFun::NativePath(cstring fn)
//{
//    const constexpr cstring var_appimg = "APPIMAGE_DATA_DIR";
//    const constexpr cstring var_snappy = "SNAP";

//    if(fn[0] == '/')
//        return fn;

//    if(AssetApi::GetAsset(fn))
//    {
//        /* Check if it's an AppImage */
//        if(Env::ExistsVar(var_appimg))
//        {
//            cstring asset = AssetApi::GetAsset(fn);
//            CString res = Env::GetVar(var_appimg);
//            CString out = Env::ConcatPath(res.c_str(),asset);
//            return out;
//        }
//        /* Check if it's a Snap */
//        else if(Env::ExistsVar(var_snappy))
//        {
//            cstring asset = AssetApi::GetAsset(fn);
//            if(!Env::ExistsVar(var_snappy))
//                return asset;
//            CString res = Env::GetVar(var_snappy);
//            res = Env::ConcatPath(res.c_str(),"assets");
//            CString out = Env::ConcatPath(res.c_str(),asset);
//            return out;
//        }else{
//            return AssetApi::GetAsset(fn);
//        }
//    }else
//    {
//        return FileFun::DereferenceLink(MkUrl(fn));
//    }
//}

//CString LinuxFileFun::NativePath(cstring fn, ResourceAccess storage)
//{
//    if(feval(storage,ResourceAccess::ConfigFile))
//    {
//        CString udata = Env::GetUserData(ApplicationData().organization_name.c_str(),
//                                         ApplicationData().application_name.c_str());
//        return Env::ConcatPath(udata.c_str(),fn);
//    }else if(feval(storage,ResourceAccess::TemporaryFile))
//        return Env::ConcatPath("/tmp",fn);
//    else
//        return NativePath(fn);
//}

//bool LinuxFileFun::VerifyAsset(cstring fn)
//{
//    CString native = NativePath(fn);

//    struct stat file_stat = {};

//    return stat(native.c_str(), &file_stat) == 0;
//}

#endif

CString LinuxFileFun::sys_read(cstring fn, bool quiet)
{
    CString out;
    FILE* fh = fopen(fn,"r");
    char* arg = 0;
    size_t size = 0;

    if(!fh && !quiet)
    {
        ErrnoCheck(fn, -1);
        return out;
    }

#if !defined(COFFEE_ANDROID)
    while(getdelim(&arg,&size,0,fh) != -1)
    {
        out.append(arg);
    }

    free(arg);
    if(out.size() > 0)
        out.resize(out.size()-1);
#else
    /* Source:
     * https://stackoverflow.com/questions/12237712/how-can-i-show-the-size-of-files-in-proc-it-should-not-be-size-zero */
    do {
        char linedata[256];
        memset(linedata, 0, sizeof(linedata));
        if(fgets(linedata, sizeof(linedata), fh) == nullptr)
            break;
        out.append(linedata);
    } while(!feof(fh));
#endif
    fclose(fh);
    return out;
}

}

}
}
