#include <coffee/core/base/files/url.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/files/cfiles.h>

#include "../plat/file/file_abstraction.h"
#include <coffee/core/plat/plat_file.h>

#include <coffee/core/coffee.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

namespace Coffee{

using namespace CResources;
static CString _coffee_resource_prefix = "./";

struct SystemPaths
{
    Url assetDir;
    Url cacheDir;
    Url configDir;
    Url tempDir;
};

STATICINLINE SystemPaths GetSystemPaths()
{
    SystemPaths paths;

#if defined(COFFEE_ANDROID)

    AndroidForeignCommand cmd;
    cmd.type =  Android_QueryDataPath;
    cmd.store_string = {};

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr, nullptr);

    paths.configDir = MkUrl(cmd.store_string.c_str(),
                            RSCA::SpecifyStorage|RSCA::SystemFile);

    paths.assetDir = MkUrl("", RSCA::SpecifyStorage|RSCA::AssetFile);

    cmd.type = Android_QueryCachePath;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr, nullptr);

    paths.cacheDir = MkUrl(cmd.store_string.c_str(),
                           RSCA::SpecifyStorage|RSCA::SystemFile);

    paths.tempDir = paths.cacheDir;

#elif defined(COFFEE_LINUX)

    /* Asset directories may be in AppImages or Snaps */
    /* TODO: Add the case for Flatpak */
    const constexpr cstring var_appimg = "APPIMAGE_DATA_DIR";
    const constexpr cstring var_snappy = "SNAP";

    if(Env::ExistsVar(var_appimg))
        paths.assetDir = MkUrl(Env::GetVar(var_appimg).c_str(),
                               RSCA::SystemFile);
    else if(Env::ExistsVar(var_snappy))
        paths.assetDir = MkUrl(Env::GetVar(var_snappy).c_str(),
                               RSCA::SystemFile) + Path{"assets"};
    else if(_coffee_resource_prefix.size() > 0)
        paths.assetDir = MkUrl(_coffee_resource_prefix.c_str(),
                               RSCA::SystemFile);
    else
        paths.assetDir = MkUrl(Env::CurrentDir().c_str(),
                               RSCA::SystemFile);

    /* Cache goes in ~/.cache/ORGNAME/APPNAME */
    paths.cacheDir = MkUrl(Env::GetUserHome().c_str(), RSCA::SystemFile)
            + Path{".cache"} + Path{ApplicationData().organization_name}
            + Path{ApplicationData().application_name};

    /* Temporary files go in /tmp */
    paths.tempDir = MkUrl("/tmp", RSCA::SystemFile)
            + Path{ApplicationData().application_name};

    paths.configDir = MkUrl(Env::GetUserData(nullptr, nullptr).c_str(),
                            RSCA::SystemFile);
#else

#endif

    return paths;
}

CString Url::operator*() const
{
    switch(category)
    {
    case Local:
    {
        if(cachedUrl.size())
            return cachedUrl;

#if defined(COFFEE_UNIXPLAT)
        auto derefPath = DereferenceLocalPath();
        derefPath = CStrReplace(derefPath, "//", "/");
        return derefPath;
#else
        return DereferenceLocalPath();
#endif
    }
    case Networked:
        return internUrl;
    default:
        throw std::runtime_error("dereferencing Url without category");
        return {};
    }
}

CString Url::operator*()
{
    Url const& self = *this;
    auto deref = *self;

    cachedUrl = deref;
    return deref;
}

Resource Url::rsc() const
{
    return CResources::Resource(*this);
}

Url Url::operator+(const Path &path) const
{
    Url cpy = *this;

    if(internUrl.size())
        cpy.internUrl = Env::ConcatPath(internUrl.c_str(),
                                        path.internUrl.c_str());
    else
        cpy.internUrl = path.internUrl;
    cpy.cachedUrl = {};
    return cpy;
}

STATICINLINE CString DereferencePath(cstring suffix,
                                     ResourceAccess storageMask)
{
    if(feval(storageMask & RSCA::SystemFile))
        return suffix;

    auto paths = GetSystemPaths();

    auto urlPart = Path{suffix};

    switch(storageMask ^ RSCA::SpecifyStorage)
    {
    case RSCA::AssetFile:
    {  
#if defined(COFFEE_ANDROID)
        /* Because Android uses a virtual filesystem,
         *  we do not go deeper to find a RSCA::SystemFile URL */
        return urlPart.internUrl.c_str();
#else
        return *(paths.assetDir + urlPart);
#endif
    }
    case RSCA::ConfigFile:
    {
        return *(paths.configDir + urlPart);
    }
    case RSCA::TemporaryFile:
    {
        return *(paths.tempDir + urlPart);
    }
    case RSCA::CachedFile:
    {
        return *(paths.cacheDir + urlPart);
    }
    default:
    {
        /* In this case, we have no preference. Default to asset. */
        return {};
    }
    }



//    //We will NOT try to add any '/' in there.
//    cVerbose(9, "Dereferencing resource path");
//    if(feval(storageMask&ResourceAccess::SpecifyStorage))
//    {
////        CString asset_fn = CString(AssetApi::AssetPrefix)+suffix;

//        if(feval(storageMask&ResourceAccess::ConfigFile))
//        {
//#if defined(COFFEE_APPLE_MOBILE)
//            return FileFun::NativePath(
//                        asset_fn.c_str(),
//                        storageMask&ResourceAccess::StorageMask);
//#else
//            cVerbose(9, "Detected configuration file flag");
//            CString cfgDir = Env::GetUserData(nullptr,nullptr);
//            cVerbose(9, "Using configuration directory: {0}", cfgDir);
//            if(!DirFun::MkDir(MkUrl(cfgDir.c_str()), true))
//                return {};
//            return Env::ConcatPath(cfgDir.c_str(),suffix);
//#endif
//        }
//#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE) || \
//    defined(COFFEE_LINUX) || defined(COFFEE_WINDOWS)
//        else if(feval(storageMask,ResourceAccess::AssetFile)
//                && FileFun::VerifyAsset(suffix))
//            return suffix;
//#endif
//        else if(feval(storageMask,ResourceAccess::TemporaryFile))
//            return
//                    FileFun::NativePath(
//                        suffix,ResourceAccess::TemporaryFile);
//        else if(feval(storageMask,ResourceAccess::SystemFile))
//            return FileFun::NativePath(
//                        suffix, ResourceAccess::SystemFile);
//    }
//#if defined(COFFEE_ANDROID)
//    return suffix;
//#else
//    return _coffee_resource_prefix + suffix;
//#endif
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
