#include <coffee/core/base/files/url.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

#include "../plat/file/file_abstraction.h"
#include <coffee/core/plat/plat_file.h>

namespace Coffee{

using namespace CResources;

CString Url::operator*() const
{
    switch(category)
    {
    case Local:
        return DereferenceLocalPath();
    case Networked:
        return internUrl;
    default:
        return {};
    }
}

Resource Url::rsc() const
{
    return CResources::Resource(*this);
}

static CString _coffee_resource_prefix = "./";

STATICINLINE CString DereferencePath(cstring suffix,
                                     ResourceAccess storageMask)
{
    //We will NOT try to add any '/' in there.
    cVerbose(9, "Dereferencing resource path");
    if(feval(storageMask&ResourceAccess::SpecifyStorage))
    {
        CString asset_fn = CString(AssetApi::AssetPrefix)+suffix;

        if(feval(storageMask&ResourceAccess::ConfigFile))
        {
#if defined(COFFEE_APPLE_MOBILE)
            return FileFun::NativePath(
                        asset_fn.c_str(),
                        storageMask&ResourceAccess::StorageMask);
#else
            cVerbose(9, "Detected configuration file flag");
            CString cfgDir = Env::GetUserData(nullptr,nullptr);
            cVerbose(9, "Using configuration directory: {0}", cfgDir);
            if(!DirFun::MkDir(cfgDir.c_str(), true))
                return {};
            return Env::ConcatPath(cfgDir.c_str(),suffix);
#endif
        }
#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE) || \
    defined(COFFEE_LINUX) || defined(COFFEE_WINDOWS)
        else if(feval(storageMask,ResourceAccess::AssetFile)
                && FileFun::VerifyAsset(asset_fn.c_str()))
            return asset_fn.c_str();
#endif
        else if(feval(storageMask,ResourceAccess::TemporaryFile))
            return FileFun::NativePath(suffix,ResourceAccess::TemporaryFile);
    }
#if defined(COFFEE_ANDROID)
    return suffix;
#else
    return _coffee_resource_prefix + suffix;
#endif
}

CString Url::DereferenceLocalPath() const
{
    return DereferencePath(internUrl, flags & ResourceAccess::StorageMask);
}

void CResources::FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

}
