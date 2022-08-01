#include <url/url.h>

#include <coffee/core/resource_prefix.h>
#include <peripherals/error/file_base.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/regex.h>
#include <peripherals/stl/stlstring_ops.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/pimpl_state.h>
#include <platforms/profiling.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>

#elif defined(COFFEE_WINDOWS_UWP)
#pragma comment(lib, "windowsapp")

#include <ntverp.h>

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;

using WPkg = ::Windows::ApplicationModel::Package;

#elif defined(COFFEE_APPLE)
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>
#endif

using namespace ::Coffee;
using namespace ::enum_helpers;

using Profiler     = platform::profiling::SimpleProfilerImpl;
using DProfContext = platform::profiling::DeepProfilerContext;

namespace platform {
namespace url {

using namespace url::constructors;

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

    paths.assetDir  = MkInvalidUrl();
    paths.cacheDir  = MkInvalidUrl();
    paths.configDir = MkInvalidUrl();
    paths.tempDir   = MkInvalidUrl();

    CString const& _coffee_resource_prefix = file::ResourcePrefix();

    auto& appData = *state->GetAppData();

#if defined(COFFEE_ANDROID)

    auto app_info = android::app_info();

    paths.configDir = app_info.data_path();

    paths.cacheDir = app_info.cache_path();
    paths.tempDir  = paths.cacheDir;
#elif defined(COFFEE_LINUX)

    /* Asset directories may be in AppImages or Snaps */

    /* TODO: Add the case for Flatpak */
    if(auto appimg = env::var("APPIMAGE_DATA_DIR"); appimg.has_value())
        paths.assetDir = MkSysUrl(appimg.value());
    else if(auto snappy = env::var("SNAP"); snappy.has_value())
        paths.assetDir = MkSysUrl(snappy.value()) / "assets";
    else if(_coffee_resource_prefix.size() > 0)
        paths.assetDir = MkSysUrl(_coffee_resource_prefix);
    else
        paths.assetDir = path::current_dir().value();

    /* Cache goes in ~/.cache/ORGNAME/APPNAME */
    paths.cacheDir = env::home_dir().value() / ".cache" /
                     appData.organization_name / appData.application_name;

    /* Temporary files go in /tmp */
    paths.tempDir = MkSysUrl("/tmp") / appData.application_name;

    if(auto snappy = env::var("SNAP_USER_COMMON"); snappy.has_value())
        paths.configDir = MkSysUrl(snappy.value());
    else
    {
        auto& orgname = appData.organization_name;
        auto& appname = appData.application_name;

        paths.configDir =
            env::home_dir().value() / ".local" / "share" / orgname / appname;
    }

#elif defined(COFFEE_WINDOWS)

    auto GetCurrentApp = platform::state->GetAppData;

    paths.assetDir = MkUrl(_coffee_resource_prefix, RSCA::SystemFile);

    CString temp_dir = Env::GetVar("TEMP");
    paths.tempDir    = MkUrl(temp_dir.c_str(), RSCA::SystemFile) +
                    Path{GetCurrentApp()->organization_name} +
                    Path{GetCurrentApp()->application_name};

    CString config_dir = Env::GetVar("APPDATA");
    paths.configDir    = MkUrl(config_dir.c_str(), RSCA::SystemFile) +
                      Path{GetCurrentApp()->organization_name} +
                      Path{GetCurrentApp()->application_name};

    CString cache_dir = Env::GetVar("LOCALAPPDATA");
    paths.cacheDir    = MkUrl(cache_dir.c_str(), RSCA::SystemFile) +
                     Path{GetCurrentApp()->organization_name} +
                     Path{GetCurrentApp()->application_name};

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

    if(_coffee_resource_prefix.size() > 0)
        paths.assetDir = MkUrl(_coffee_resource_prefix, RSCA::SystemFile);
    else
        paths.assetDir = GetAppleStoragePath();

    auto home = Env::GetVar("HOME");
#if defined(COFFEE_APPLE_MOBILE)

    auto base = MkUrl(home.c_str(), RSCA::SystemFile);

    paths.tempDir   = base + Path{"tmp"};
    paths.configDir = base + Path{"Documents"};
    paths.cacheDir  = base + Path{"Library"} + Path{"Caches"};

#else
    paths.tempDir =
        MkUrl("/tmp", RSCA::SystemFile) + Path(appData.application_name);

    auto library = MkUrl(home.c_str(), RSCA::SystemFile) + Path{"Library"};

    auto app_path =
        Path{appData.organization_name} + Path{appData.application_name};

    paths.configDir = library + Path{"Application Support"} + app_path;

    paths.cacheDir = library + Path{"Caches"} + app_path;
#endif

#elif defined(COFFEE_EMSCRIPTEN)

    /* Emscripten uses a virtual filesystem */

    paths.assetDir  = MkUrl("/assets", RSCA::SystemFile);
    paths.cacheDir  = MkUrl("/cache", RSCA::SystemFile);
    paths.configDir = MkUrl("/config", RSCA::SystemFile);
    paths.tempDir   = MkUrl("/tmp", RSCA::SystemFile);

#elif defined(COFFEE_GEKKO)

    /* Corresponds to memory cards on GC */
    paths.configDir = MkUrl("MEM0:/", RSCA::SystemFile);
    // TODO: Should correspond to block storage on Wii

    /* Corresponds to GC/Wii storage media */
    paths.assetDir = MkUrl("GCM:/", RSCA::SystemFile);

    /* Emulated as in-memory storage */
    paths.tempDir  = MkUrl("TMP:/", RSCA::SystemFile);
    paths.cacheDir = paths.tempDir;

#else
#error Unimplemented filesystem path handling, fix it!
#endif

    return paths;
}

#if defined(COFFEE_ANDROID)
/*!
 * \brief Get a system-directed file path
 * This is used on Android for creating temporary files.
 * \return
 */
CString GetSystemDirectedPath(cstring suffix, RSCA storage)
{
    DProfContext _("JNI path resolution");

    using namespace ::jnipp_operators;

    //    using FileType = JavaClass<javaioFile, void, void>;

    android::ScopedJNI jni(jnipp::GetVM());
    jnipp::SwapJNI(&jni);

    auto File           = "java.io.File"_jclass;
    auto createTempFile = "createTempFile"_jmethod.ret("java.io.File")
                              .arg<std::string>("java.lang.String")
                              .arg<std::string>("java.lang.String");
    auto getCanonicalPath = "getCanonicalPath"_jmethod.ret("java.lang.String");

    switch(storage & RSCA::StorageMask)
    {
    case RSCA::TempFile: {
        Path filePath(suffix);

        auto fileInstance = File[createTempFile](
            filePath.removeExt().internUrl, "." + filePath.extension());

        auto tempFile = File(fileInstance)[getCanonicalPath]();

        return jnipp::java::type_unwrapper<std::string>(tempFile);
    }
    default:
        return {};
    }
    //    auto var = File[createTempFile]({});
}
#endif

CString Url::operator*() const
{
    switch(category)
    {
    case Local: {
        if(cachedUrl.size())
            return cachedUrl;

        String derefPath = DereferenceLocalPath();
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_EMSCRIPTEN)
        derefPath = str::replace::str<char>(derefPath, "//", "/");
        if(feval(flags, RSCA::NoDereference))
            return derefPath;
        if(auto path =
               path::dereference(MkUrl(derefPath.c_str(), RSCA::SystemFile));
           path.has_error())
            return derefPath;
        else
            return path.value().internUrl;
#else
        derefPath = str::replace::str<char>(derefPath, "\\", "/");
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
    if(!cachedUrl.size())
    {
        Url const& self  = *this;
        auto       deref = *self;

        cachedUrl = deref;
    }
    return cachedUrl;
}

Url Url::operator+(const Path& path) const
{
    Url cpy = *this;

    if(internUrl.size())
        cpy.internUrl = internUrl + path::path_separator + path.internUrl;
    else
        cpy.internUrl = path.internUrl;

    cpy.cachedUrl = {};
    return cpy;
}

STATICINLINE CString DereferencePath(cstring suffix, RSCA storageMask)
{
    if(feval(storageMask & RSCA::SystemFile))
        return suffix;

#if !defined(COFFEE_GEKKO)
    auto paths = GetSystemPaths();

    auto urlPart = Path{suffix};

#if defined(COFFEE_APPLE_MOBILE)
    /* Hey ho! On iOS we strip all directory structures, fun to debug, right? */
    urlPart = urlPart.fileBasename();
#endif

    Url tempStore = {};

    switch(storageMask & RSCA::StorageMask)
    {
    case RSCA::AssetFile: {
#if defined(COFFEE_VIRTUAL_ASSETS)
        /* Because Android uses a virtual filesystem,
         *  we do not go deeper to find a RSCA::SystemFile URL */
        return urlPart.internUrl.c_str();
#else
        tempStore = paths.assetDir + urlPart;
        break;
#endif
    }
    case RSCA::ConfigFile: {
        file::create_directory(paths.configDir, {.recursive = true});
        tempStore = paths.configDir + urlPart;
        break;
    }
    case RSCA::TemporaryFile: {
#if defined(COFFEE_DYNAMIC_TEMPFILES)
        tempStore =
            MkUrl(GetSystemDirectedPath(suffix, storageMask), RSCA::SystemFile);
#else
        file::create_directory(paths.tempDir, {.recursive = true});
        tempStore = paths.tempDir + urlPart;
#endif
        break;
    }
    case RSCA::CachedFile: {
#if defined(COFFEE_DYNAMIC_TEMPFILES)
        tempStore =
            MkUrl(GetSystemDirectedPath(suffix, storageMask), RSCA::SystemFile);
#else
        file::create_directory(paths.cacheDir, {.recursive = true});
        tempStore = paths.cacheDir + urlPart;
#endif
        break;
    }
    default: {
        /* In this case, we have no preference. Default to asset. */
        return {};
    }
    }
    tempStore.flags |= storageMask & RSCA::NoDereference;
    return *tempStore;
#else
    switch(storageMask & RSCA::StorageMask)
    {
    default:
        Throw(undefined_behavior("failed to resolve path"));
    }

#endif
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
    return Path{internUrl.substr(0, it)};
}

Path Path::addExtension(cstring ext) const
{
    return Path{(internUrl + ".") + ext};
}

Path Path::fileBasename() const
{
    Path p(path::base(MkUrl(internUrl)).value().internUrl);
    return p;
}

CString Path::extension() const
{
    auto it     = internUrl.rfind(".");
    auto sep_it = internUrl.rfind(path::path_separator);

    if(it == CString::npos || (sep_it != CString::npos && sep_it > it))
        return {};

    return internUrl.substr(it + 1, internUrl.size() - it - 1);
}

Path Path::dirname() const
{
    Path p(path::dir({internUrl}).value().internUrl);
    return p;
}

Vector<Path> Path::components() const
{
    Vector<Path> out;

    Path base = *this;

    while(base != Path("."))
    {
        out.push_back(base);
        base = base.dirname();
    }

    return out;
}

Path Path::operator+(cstring component) const
{
    Path cpy = *this;
    if(cpy.internUrl.size())
        cpy.internUrl = cpy.internUrl + path::path_separator + component;
    else
        cpy.internUrl = component;
    return cpy;
}

Path Path::operator+(const Path& path) const
{
    if(internUrl.size())
        return Path{internUrl + path::path_separator + path.internUrl};
    else
        return path;
}

Path& Path::operator=(const Url& url)
{
    internUrl = url.internUrl;
    return *this;
}

#define URLPARSE_TAG "UrlParse::From"
#define URLPARSE_CHARS ""

struct UrlParseCache : GlobalState
{
    virtual ~UrlParseCache();
    regex::Pattern pattern;
};

UrlParseCache::~UrlParseCache()
{
}

UrlParseCache& GetCache()
{
    constexpr cstring url_pattern =
        "^"                                     /* from start */
        "([A-Za-z0-9\\.]*[A-Za-z0-9])://"       /* protocol */
        "([A-Za-z0-9-_$\\.\\,\\+!\\*'\\(\\)]+)" /* host */
        "(:([0-9]+)/(.*)|:([0-9]+)|/(.+)|)"     /* port and resource */
        "$"                                     /* to end */
        ;

    constexpr cstring pattern_key = "urlParsePattern";

    auto ptr = state->PeekState(pattern_key);

    if(ptr)
        return *C_DCAST<UrlParseCache>(ptr.get());

    auto patt = MkShared<UrlParseCache>();

    Profiler::DeepPushContext(URLPARSE_TAG "Regex compile");
    patt->pattern = regex::compile_pattern(url_pattern);
    Profiler::DeepPopContext();

    state->SwapState(pattern_key, patt);

    return *C_DCAST<UrlParseCache>(patt.get());
}

UrlParse UrlParse::From(const Url& url)
{
    enum RegexEnum
    {
        URL_Proto = 1,
        URL_Host,
        URL_PortWithResource,
        URL_Port,
        URL_PortResource,
        URL_PortOnly,
        URL_Resource,
    };

    DProfContext _(URLPARSE_TAG);
    UrlParse     p = {};

#if !defined(COFFEE_IOS)

    Vector<CString> matches;

    {
        DProfContext _(URLPARSE_TAG "Regex");
        if(!regex::match(GetCache().pattern, *url, matches))
            return p;
    }

    p.m_protocol = matches[1];
    p.m_host     = matches[2];

    if(matches[URL_Port].size())
    {
        p.m_port     = cast_string<u32>(matches[URL_Port]);
        p.m_resource = matches[URL_PortResource];
    } else if(matches[URL_PortOnly].size())
    {
        p.m_port = cast_string<u32>(matches[URL_PortOnly]);
    } else
    {
        p.m_port     = 0;
        p.m_resource = matches[URL_Resource];
    }
#else
    p.m_protocol = "https";
    p.m_port     = 0;

    auto urlData     = *url;
    auto protocolEnd = urlData.find("://");
    auto hasProtocol = protocolEnd != CString::npos;

    auto host        = urlData.substr(hasProtocol ? protocolEnd + 3 : 0);
    auto hostEnd     = host.find("/");
    auto hasResource = hostEnd != CString::npos;
    auto portStart   = host.find(":");

    if(portStart != CString::npos)
    {
        p.m_port = cast_string<u32>(host.substr(
            portStart + 1, hasResource ? hostEnd - portStart : CString::npos));
    }

    if(hostEnd != CString::npos)
    {
        p.m_resource = host.substr(hostEnd);
    }

    p.m_protocol = urlData.substr(0, protocolEnd);
    p.m_host     = host.substr(0, hostEnd);

#endif
    return p;
}

} // namespace url
} // namespace platform

namespace Coffee {
namespace Strings {
using namespace ::stl_types;
using namespace ::platform::url;

CString to_string(const Path& path)
{
    return "path(" + path.internUrl + ")";
}

CString to_string(const Url& url)
{
    return "url(" + url.internUrl + "," +
           str::print::pointerify(C_CAST<u32>(url.flags)) + ")";
}
} // namespace Strings
} // namespace Coffee
