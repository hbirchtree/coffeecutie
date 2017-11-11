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

Url Url::operator+(const Path &path) const
{
    Url cpy = *this;
    cpy.internUrl = Env::ConcatPath(internUrl.c_str(),
                                    path.internUrl.c_str());
    return *this;
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
            if(!DirFun::MkDir(MkUrl(cfgDir.c_str()), true))
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
            return
                    FileFun::NativePath(
                        suffix,ResourceAccess::TemporaryFile);
        else if(feval(storageMask,ResourceAccess::SystemFile))
            return FileFun::NativePath(
                        suffix, ResourceAccess::SystemFile);
    }
#if defined(COFFEE_ANDROID)
    return suffix;
#else
    return _coffee_resource_prefix + suffix;
#endif
}

CString Url::DereferenceLocalPath() const
{
    return DereferencePath(internUrl.c_str(),
                           flags & ResourceAccess::StorageMask);
}

void CResources::FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

Path Path::removeExt()
{
    auto it = internUrl.rfind(".");
    if(it != CString::npos)
        return {};
    return {internUrl.substr(0, it)};
}

Path Path::addExtension(cstring ext)
{
    return {(internUrl + ".") + ext};
}

Path Path::fileBasename()
{
    return {DirFun::Basename(internUrl.c_str())};
}

Path Path::dirname()
{
    return {Env::DirName(internUrl.c_str())};
}

Path Path::operator+(cstring component)
{
    internUrl += component;
    return *this;
}

Path Path::operator+(const Path &path)
{
    return {Env::ConcatPath(internUrl.c_str(),
                            path.internUrl.c_str())};
}

Path &Path::operator=(const Url &url)
{
    internUrl = url.internUrl;
    return *this;
}

namespace Strings{
CString to_string(const Path &path)
{
    return "path(" + path.internUrl + ")";
}

CString to_string(const Url &url)
{
    return "url(" + url.internUrl + ","
            + StrUtil::pointerify(C_FCAST<u32>(url.flags))
            + ")";
}
}
}
