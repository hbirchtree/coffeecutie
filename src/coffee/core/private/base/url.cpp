#include <coffee/core/base/files/url.h>

#include "../plat/file/file_abstraction.h"
#include <coffee/core/base/files/cfiles.h>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_resource.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/plat_file.h>

#include <coffee/core/CDebug>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_WINDOWS_UWP)

#pragma comment(lib, "windowsapp")

#include <ntverp.h>

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;

using WPkg = ::Windows::ApplicationModel::Package;

#endif

#if defined(COFFEE_APPLE)
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>
#endif

namespace Coffee {

using namespace CResources;

struct SystemPaths
{
    Url assetDir;
    Url cacheDir;
    Url configDir;
    Url tempDir;
};

#if defined(COFFEE_APPLE)
Url GetAppleStoragePath()
{
    Url out = {};
    do
    {
        CFBundleRef bun = CFBundleGetMainBundle();
        if(!bun)
            break;
        CFURLRef    path = CFBundleCopyBundleURL(bun);
        CFStringRef pathstr =
            CFURLCopyFileSystemPath(path, kCFURLPOSIXPathStyle);
        CFStringEncoding enc      = CFStringGetSystemEncoding();
        const char*      pathcstr = CFStringGetCStringPtr(pathstr, enc);
        CString          outStr   = pathcstr;
        CFRelease(pathstr);
        CFRelease(path);
#if defined(COFFEE_APPLE_MOBILE)
        out = MkUrl(outStr.c_str(), RSCA::SystemFile);
#else
        out = MkUrl(outStr.c_str(), RSCA::SystemFile) + Path{"Contents"} +
              Path{"Resources"};
#endif
    } while(false);
    return out;
}
#endif

STATICINLINE SystemPaths GetSystemPaths()
{
    SystemPaths paths;

    CString const& _coffee_resource_prefix = GetFileResourcePrefix();

#if defined(COFFEE_ANDROID)

    AndroidForeignCommand cmd;
    cmd.type         = Android_QueryDataPath;
    cmd.store_string = {};

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    paths.configDir = MkUrl(
        cmd.store_string.c_str(), RSCA::SpecifyStorage | RSCA::SystemFile);

    cmd.type = Android_QueryCachePath;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    paths.cacheDir = MkUrl(
        cmd.store_string.c_str(), RSCA::SpecifyStorage | RSCA::SystemFile);

    paths.tempDir = paths.cacheDir;

#elif defined(COFFEE_LINUX)

    /* Asset directories may be in AppImages or Snaps */
    /* TODO: Add the case for Flatpak */
    const constexpr cstring var_appimg = "APPIMAGE_DATA_DIR";
    const constexpr cstring var_snappy = "SNAP";

    if(Env::ExistsVar(var_appimg))
        paths.assetDir =
            MkUrl(Env::GetVar(var_appimg).c_str(), RSCA::SystemFile);
    else if(Env::ExistsVar(var_snappy))
        paths.assetDir =
            MkUrl(Env::GetVar(var_snappy).c_str(), RSCA::SystemFile) +
            Path{"assets"};
    else if(_coffee_resource_prefix.size() > 0)
        paths.assetDir =
            MkUrl(_coffee_resource_prefix.c_str(), RSCA::SystemFile);
    else
        paths.assetDir = Env::CurrentDir();

    /* Cache goes in ~/.cache/ORGNAME/APPNAME */
    paths.cacheDir = Env::GetUserHome() + Path{".cache"} +
                     Path{GetCurrentApp().organization_name} +
                     Path{GetCurrentApp().application_name};

    /* Temporary files go in /tmp */
    paths.tempDir = MkUrl("/tmp", RSCA::SystemFile) +
                    Path{GetCurrentApp().application_name};

    paths.configDir = Env::GetUserData(nullptr, nullptr);

#elif defined(COFFEE_WINDOWS)

    CString temp_dir = Env::GetVar("TEMP");
    paths.tempDir    = MkUrl(temp_dir.c_str(), RSCA::SystemFile) +
                    Path{GetCurrentApp().organization_name} +
                    Path{GetCurrentApp().application_name};

    CString config_dir = Env::GetVar("APPDATA");
    paths.configDir    = MkUrl(config_dir.c_str(), RSCA::SystemFile) +
                      Path{GetCurrentApp().organization_name} +
                      Path{GetCurrentApp().application_name};

    CString cache_dir = Env::GetVar("LOCALAPPDATA");
    paths.cacheDir    = MkUrl(cache_dir.c_str(), RSCA::SystemFile) +
                     Path{GetCurrentApp().organization_name} +
                     Path{GetCurrentApp().application_name};

#if defined(COFFEE_WINDOWS_UWP)
    auto pkg     = ::Windows::ApplicationModel::Package::Current();
    auto appdata = pkg.InstalledLocation().Path();

    CString bs =
        StrUtil::convertformat<char, wchar_t>(CWString(appdata.data()));

    paths.assetDir = MkUrl(bs.c_str(), RSCA::SystemFile);
#endif

    /* assetDir is supposed to be empty, as it refers to a virtual filesystem */
    /* We might want to toggle it based on running in UWP or something... */

#if VER_PRODUCTBUILD >= 17025 && defined(COFFEE_WINDOWS_UWP)
#error ERROR ERROR TIME TO IMPLEMENT PROPER UWP ASSET STORAGE M8
#endif

#elif defined(COFFEE_APPLE)

    paths.assetDir = MkUrl(_coffee_resource_prefix.c_str(), RSCA::SystemFile);

    auto home = Env::GetVar("HOME");
#if defined(COFFEE_APPLE_MOBILE)

    auto base = MkUrl(home.c_str(), RSCA::SystemFile);

    paths.tempDir   = base + Path{"tmp"};
    paths.configDir = base + Path{"Documents"};
    paths.cacheDir  = base + Path{"Library"} + Path{"Caches"};

#else
    paths.tempDir = MkUrl("/tmp", RSCA::SystemFile);

    auto library = MkUrl(home.c_str()) + Path{"Library"};

    auto app_path = Path{GetCurrentApp().organization_name} +
                    Path{GetCurrentApp().application_name};

    paths.configDir = library + Path{"Application Support"} + app_path;

    paths.configDir = library + Path{"Caches"} + app_path;
#endif

#elif defined(COFFEE_EMSCRIPTEN)

    /* Emscripten uses a virtual filesystem */

    paths.assetDir = MkUrl("/assets", RSCA::SystemFile);

    paths.configDir = MkUrl("/config", RSCA::SystemFile);

    paths.tempDir  = MkUrl("/tmp", RSCA::SystemFile);
    paths.cacheDir = paths.tempDir;

#elif defined(COFFEE_GEKKO)

    paths.configDir = MkUrl("/MEM0", RSCA::SystemFile);

#else
#error Unimplemented filesystem path handling, fix it!
#endif

    return paths;
}

CString Url::operator*() const
{
    switch(category)
    {
    case Local:
    {
        file_error ec;

        if(cachedUrl.size())
            return cachedUrl;

#if defined(COFFEE_UNIXPLAT)
        CString derefPath = DereferenceLocalPath();
        derefPath         = CStrReplace(derefPath, "//", "/");
#if !defined(COFFEE_EMSCRIPTEN)
        if(!feval(flags & RSCA::NoDereference))
            derefPath = FileFun::DereferenceLink(
                MkUrl(derefPath.c_str(), RSCA::SystemFile), ec);
#ifndef NDEBUG
        if(ec)
            cWarning("{0}", ec.message());
#endif
#endif
        return derefPath;
#else
        CString derefPath = DereferenceLocalPath();
        derefPath         = CStrReplace(derefPath, "\\", "/");
        return derefPath;
#endif
    }
    case Networked:
        return internUrl;
    default:
        Throw(resource_error("dereferencing Url without category"));
    }
}

CString Url::operator*()
{
    Url const& self  = *this;
    auto       deref = *self;

    cachedUrl = deref;
    return deref;
}

Url Url::operator+(const Path& path) const
{
    Url cpy = *this;

    if(internUrl.size())
        cpy.internUrl =
            Env::ConcatPath(internUrl.c_str(), path.internUrl.c_str());
    else
        cpy.internUrl = path.internUrl;

    cpy.cachedUrl = {};
    return cpy;
}

STATICINLINE CString DereferencePath(cstring suffix, RSCA storageMask)
{
    if(feval(storageMask & RSCA::SystemFile))
        return suffix;

    file_error ec;
    auto       paths = GetSystemPaths();

    auto urlPart = Path{suffix};

#if defined(COFFEE_APPLE_MOBILE)
    /* Hey ho! On iOS we strip all directory structures, fun to debug, right? */
    urlPart = urlPart.fileBasename();
#endif

    Url tempStore = {};

    switch(storageMask & RSCA::StorageMask)
    {
    case RSCA::AssetFile:
    {
#if defined(COFFEE_ANDROID) || \
    (defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP))
        /* Because Android uses a virtual filesystem,
         *  we do not go deeper to find a RSCA::SystemFile URL */
        return urlPart.internUrl.c_str();
#else
        tempStore = paths.assetDir + urlPart;
        break;
#endif
    }
    case RSCA::ConfigFile:
    {
        DirFun::MkDir(paths.configDir, true, ec);
        tempStore = paths.configDir + urlPart;
        break;
    }
    case RSCA::TemporaryFile:
    {
        DirFun::MkDir(paths.tempDir, true, ec);
        tempStore = paths.tempDir + urlPart;
        break;
    }
    case RSCA::CachedFile:
    {
        DirFun::MkDir(paths.cacheDir, true, ec);
        tempStore = paths.cacheDir + urlPart;
        break;
    }
    default:
    {
        /* In this case, we have no preference. Default to asset. */
        return {};
    }
    }
#ifndef NDEBUG
    if(ec)
        cWarning("{0}", ec.message());
#endif
    tempStore.flags |= storageMask & RSCA::NoDereference;
    return *tempStore;
}

CString Url::DereferenceLocalPath() const
{
    return DereferencePath(
        internUrl.c_str(), flags & (RSCA::StorageMask | RSCA::NoDereference));
}

Path Path::removeExt() const
{
    auto it = internUrl.rfind(".");
    if(it == CString::npos)
        return {};
    return {internUrl.substr(0, it)};
}

Path Path::addExtension(cstring ext) const
{
    return {(internUrl + ".") + ext};
}

Path Path::fileBasename() const
{
    file_error ec;
    Path       p = {DirFun::Basename(internUrl, ec).internUrl};
#ifndef NDEBUG
    if(ec)
        cWarning("{0}", ec.message());
#endif
    return p;
}

CString Path::extension() const
{
    auto it     = internUrl.rfind(".");
    auto sep_it = internUrl.rfind(DirFun::GetPathSep());

    if(it == CString::npos || (sep_it != CString::npos && sep_it > it))
        return {};

    return internUrl.substr(it + 1, internUrl.size() - it - 1);
}

Path Path::dirname() const
{
    file_error ec;
    Path       p = {DirFun::Dirname(internUrl.c_str(), ec).internUrl};
#ifndef NDEBUG
    if(ec)
        cWarning("{0}", ec.message());
#endif
    return p;
}

Path Path::canonical() const
{
    file_error ec;
    Path       p = {FileFun::CanonicalName(MkUrl(*this), ec)};
#ifndef NDEBUG
    if(ec)
        cWarning("{0}", ec.message());
#endif
    return p;
}

Path Path::operator+(cstring component) const
{
    Path cpy = *this;
    if(cpy.internUrl.size())
        cpy.internUrl = Env::ConcatPath(cpy.internUrl.c_str(), component);
    else
        cpy.internUrl = component;
    //    cpy.internUrl += component;
    return cpy;
}

Path Path::operator+(const Path& path) const
{
    if(internUrl.size())
        return {Env::ConcatPath(internUrl.c_str(), path.internUrl.c_str())};
    else
        return path;
}

Path& Path::operator=(const Url& url)
{
    internUrl = url.internUrl;
    return *this;
}

namespace Strings {
CString to_string(const Path& path)
{
    return "path(" + path.internUrl + ")";
}

CString to_string(const Url& url)
{
    return "url(" + url.internUrl + "," +
           StrUtil::pointerify(C_CAST<u32>(url.flags)) + ")";
}
} // namespace Strings
} // namespace Coffee
