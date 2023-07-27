#include <url/url.h>

#include <coffee/core/resource_prefix.h>
#include <peripherals/error/file_base.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/regex.h>
#include <peripherals/stl/stlstring_ops.h>
#include <peripherals/stl/string/hex.h>
#include <peripherals/stl/string/replace.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/pimpl_state.h>
#include <platforms/profiling.h>

#include <fmt/format.h>
#include <fmt/std.h>

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
#elif defined(COFFEE_EMSCRIPTEN)
#include <emscripten/em_asm.h>
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
        CFStringRef pathstr
            = CFURLCopyFileSystemPath(path, kCFURLPOSIXPathStyle);
        CFStringEncoding enc      = CFStringGetSystemEncoding();
        const char*      pathcstr = CFStringGetCStringPtr(pathstr, enc);
        if(!pathcstr)
            break;
        std::string outStr = pathcstr;
        CFRelease(pathstr);
        CFRelease(path);
#if defined(COFFEE_APPLE_MOBILE)
        out = MkUrl(outStr.c_str(), RSCA::SystemFile);
#else
        out = MkUrl(outStr.c_str(), RSCA::SystemFile) / Path{"Contents"} /
              Path{"Resources"};
#endif
    } while(false);
    return out;
}
#endif

#if defined(COFFEE_EMSCRIPTEN)
static void ensure_indexeddb_filesystem_mounted()
{
    static bool mounted{false};

    if(!mounted)
    {
        // clang-format off
        EM_ASM(
            FS.mkdir("/config");
            FS.mount(IDBFS, {}, "/config");
            FS.syncfs("/config");
        );
        // clang-format on
        mounted = true;
    }
}
#endif

STATICINLINE SystemPaths& GetSystemPaths()
{
    static SystemPaths paths{};
    static bool        prefix_available{false};

    const bool valid_paths = paths.assetDir.valid() || paths.cacheDir.valid();
    if(valid_paths && prefix_available)
        return paths;

    auto prefix      = file::ResourcePrefix();
    prefix_available = prefix.has_value();

    paths.assetDir  = invalid_url();
    paths.cacheDir  = invalid_url();
    paths.configDir = invalid_url();
    paths.tempDir   = invalid_url();

    auto _coffee_resource_prefix = prefix.value_or("./");

    [[maybe_unused]] auto& appData = *state->GetAppData();

#if defined(COFFEE_ANDROID)

    auto app_info = android::app_info();

    paths.configDir = app_info.data_path();
    paths.assetDir  = app_info.external_data_path();
    paths.cacheDir  = app_info.cache_path();
    paths.tempDir   = paths.cacheDir;
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
    paths.cacheDir = env::home_dir().value() / ".cache"
                     / appData.organization_name / appData.application_name;

    /* Temporary files go in /tmp */
    paths.tempDir = "/tmp"_sys / appData.application_name;

    if(auto snappy = env::var("SNAP_USER_COMMON"); snappy.has_value())
        paths.configDir = MkSysUrl(snappy.value());
    else
    {
        auto& orgname = appData.organization_name;
        auto& appname = appData.application_name;

        paths.configDir
            = env::home_dir().value() / ".local" / "share" / orgname / appname;
    }

#elif defined(COFFEE_WINDOWS)

    auto GetCurrentApp = platform::state->GetAppData;

    paths.assetDir = MkUrl(_coffee_resource_prefix, RSCA::SystemFile);
    if(auto canon = platform::path::canon(paths.assetDir); canon.has_value())
        paths.assetDir = canon.value();
    else
        fprintf(
            stderr,
            "Failed to caonicalize asset path: %s\n",
            _coffee_resource_prefix.c_str());

    std::string temp_dir = env::var("TEMP").value_or("tmp");
    paths.tempDir        = MkUrl(temp_dir.c_str(), RSCA::SystemFile)
                    + Path{GetCurrentApp()->organization_name}
                    + Path{GetCurrentApp()->application_name};

    std::string config_dir = env::var("APPDATA").value();
    paths.configDir        = MkUrl(config_dir.c_str(), RSCA::SystemFile)
                      + Path{GetCurrentApp()->organization_name}
                      + Path{GetCurrentApp()->application_name};

    std::string cache_dir = env::var("LOCALAPPDATA").value();
    paths.cacheDir        = MkUrl(cache_dir.c_str(), RSCA::SystemFile)
                     + Path{GetCurrentApp()->organization_name}
                     + Path{GetCurrentApp()->application_name};

#if defined(COFFEE_WINDOWS_UWP)
    auto pkg     = ::Windows::ApplicationModel::Package::Current();
    auto appdata = pkg.InstalledLocation().Path();

    std::string bs
        = StrUtil::convertformat<char, wchar_t>(std::wstring(appdata.data()));

    paths.assetDir = MkUrl(bs.c_str(), RSCA::SystemFile);
#endif

    /* assetDir is supposed to be empty, as it refers to a virtual filesystem */
    /* We might want to toggle it based on running in UWP or something... */

#if VER_PRODUCTBUILD >= 17025 && defined(COFFEE_WINDOWS_UWP)
#error ERROR ERROR TIME TO IMPLEMENT PROPER UWP ASSET STORAGE M8
#endif

#elif defined(COFFEE_APPLE)

    if(prefix.has_value())
        paths.assetDir = MkUrl(*prefix, RSCA::SystemFile);
    else
        paths.assetDir = GetAppleStoragePath();

    auto home = env::var("HOME").value_or("/");
#if defined(COFFEE_APPLE_MOBILE)

    auto base = MkUrl(home.c_str(), RSCA::SystemFile);

    paths.tempDir   = base + Path{"tmp"};
    paths.configDir = base + Path{"Documents"};
    paths.cacheDir  = base + Path{"Library"} + Path{"Caches"};

#else
    paths.tempDir
        = MkUrl("/tmp", RSCA::SystemFile) + Path(appData.application_name);

    auto library = MkUrl(home.c_str(), RSCA::SystemFile) + Path{"Library"};

    auto app_path
        = Path{appData.organization_name} / Path{appData.application_name};

    paths.configDir = library + Path{"Application Support"} + app_path;

    paths.cacheDir = library + Path{"Caches"} + app_path;
#endif

#elif defined(COFFEE_EMSCRIPTEN)

    /* Emscripten uses a virtual filesystem */

    paths.assetDir  = MkUrl("/assets", RSCA::SystemFile);
    paths.cacheDir  = MkUrl("/cache", RSCA::SystemFile);
    paths.tempDir   = MkUrl("/tmp", RSCA::SystemFile);
    paths.configDir = MkUrl("/config", RSCA::SystemFile);

    ensure_indexeddb_filesystem_mounted();

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
std::string GetSystemDirectedPath(std::string suffix, RSCA storage)
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

std::string Url::operator*() const
{
    switch(category)
    {
    case Local: {
        std::string derefPath = DereferenceLocalPath();
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_EMSCRIPTEN)
        derefPath = str::replace::str<char>(derefPath, "//", "/");
        if(feval(flags, RSCA::NoDereference))
            return derefPath;
        if(auto path
           = path::dereference(MkUrl(derefPath.c_str(), RSCA::SystemFile));
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

Url Url::operator+(const Path& path) const
{
    Url cpy = *this;

    if(internUrl.size())
        cpy.internUrl = internUrl + path::path_separator + path.internUrl;
    else
        cpy.internUrl = path.internUrl;

    return cpy;
}

STATICINLINE std::string DereferencePath(std::string suffix, RSCA storageMask)
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
        tempStore = paths.assetDir + urlPart;
        break;
    }
    case RSCA::ConfigFile: {
        file::create_directory(paths.configDir, {.recursive = true});
        tempStore = paths.configDir + urlPart;
        break;
    }
    case RSCA::TemporaryFile: {
#if defined(COFFEE_DYNAMIC_TEMPFILES)
        tempStore = MkUrl(
            GetSystemDirectedPath(suffix, storageMask), RSCA::SystemFile);
#else
        file::create_directory(paths.tempDir, {.recursive = true});
        tempStore = paths.tempDir + urlPart;
#endif
        break;
    }
    case RSCA::CachedFile: {
#if defined(COFFEE_DYNAMIC_TEMPFILES)
        tempStore = MkUrl(
            GetSystemDirectedPath(suffix, storageMask), RSCA::SystemFile);
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

std::string Url::DereferenceLocalPath() const
{
    return DereferencePath(
        internUrl.c_str(), flags & (RSCA::StorageMask | RSCA::NoDereference));
}

Path Path::removeExt() const
{
    auto it = internUrl.rfind(".");
    if(it == std::string::npos)
        return Path{};
    return Path{internUrl.substr(0, it)};
}

Path Path::addExtension(std::string_view const& ext) const
{
    auto out = (internUrl + ".");
    out.insert(out.end(), ext.begin(), ext.end());
    return Path{out};
}

Path Path::fileBasename() const
{
    Path p(path::base(MkUrl(internUrl)).value().internUrl);
    return p;
}

std::string Path::extension() const
{
    auto it     = internUrl.rfind(".");
    auto sep_it = internUrl.rfind(path::path_separator);

    if(it == std::string::npos || (sep_it != std::string::npos && sep_it > it))
        return {};

    return internUrl.substr(it + 1, internUrl.size() - it - 1);
}

Path Path::dirname() const
{
    Path p(path::dir(MkUrl(internUrl)).value().internUrl);
    return p;
}

std::vector<Path> Path::components() const
{
    std::vector<Path> out;

    Path base = *this;

    while(base != Path("."))
    {
        out.push_back(base);
        base = base.dirname();
    }

    return out;
}

Path Path::operator/(std::string_view const& component) const
{
    if(!internUrl.empty())
    {
        auto out = internUrl + path::path_separator;
        out.insert(out.end(), component.begin(), component.end());
        return Path{out};
    } else
        return Path{component};
}

Path Path::operator/(Path const& path) const
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

Url Path::url(RSCA flags) const
{
    return {
        .internUrl = internUrl,
        .category  = Url::Local,
        .flags     = flags,
    };
}

Url Path::url(HTTPAccess flags) const
{
    return {
        .internUrl = internUrl,
        .category  = Url::Networked,
        .netflags  = flags,
    };
}

#define URLPARSE_TAG "UrlParse::From"
#define URLPARSE_CHARS ""

static regex::Pattern& GetCache()
{
    static std::optional<regex::Pattern> pattern;

    if(pattern.has_value())
        return *pattern;

    const std::string url_pattern
        = "^"                                     /* from start */
          "([A-Za-z0-9\\.]*[A-Za-z0-9])://"       /* protocol */
          "([A-Za-z0-9-_$\\.\\,\\+!\\*'\\(\\)]+)" /* host */
          "(:([0-9]+)/(.*)|:([0-9]+)|/(.+)|)"     /* port and resource */
          "$"                                     /* to end */
        ;
    Profiler::DeepPushContext(URLPARSE_TAG "Regex compile");
    pattern = regex::compile_pattern(url_pattern);
    Profiler::DeepPopContext();

    return *pattern;
}

const UrlParse UrlParse::from(const Url& url)
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

    std::vector<std::string> matches;

    {
        DProfContext _(URLPARSE_TAG "Regex");
        if(!regex::match(GetCache(), *url, matches))
            return p;
    }

    p.protocol = matches[1];
    p.host     = matches[2];

    if(matches[URL_Port].size())
    {
        p.port     = cast_string<u32>(matches[URL_Port]);
        p.resource = matches[URL_PortResource];
    } else if(matches[URL_PortOnly].size())
    {
        p.port = cast_string<u32>(matches[URL_PortOnly]);
    } else
    {
        p.port     = 0;
        p.resource = matches[URL_Resource];
    }
#else
    p.protocol = "https";
    p.port     = 0;

    auto urlData     = *url;
    auto protocolEnd = urlData.find("://");
    auto hasProtocol = protocolEnd != std::string::npos;

    auto host        = urlData.substr(hasProtocol ? protocolEnd + 3 : 0);
    auto hostEnd     = host.find("/");
    auto hasResource = hostEnd != std::string::npos;
    auto portStart   = host.find(":");

    if(portStart != std::string::npos)
    {
        p.port = cast_string<u32>(host.substr(
            portStart + 1,
            hasResource ? hostEnd - portStart : std::string::npos));
    }

    if(hostEnd != std::string::npos)
    {
        p.resource = host.substr(hostEnd);
    }

    p.protocol = urlData.substr(0, protocolEnd);
    p.host     = host.substr(0, hostEnd);

#endif
    return p;
}

} // namespace url
} // namespace platform

namespace Coffee {
namespace Strings {
using namespace ::stl_types;
using namespace ::platform::url;

std::string to_string(const Path& path)
{
    return "path(" + path.internUrl + ")";
}

std::string to_string(const Url& url)
{
    return "url(" + url.internUrl + ","
           + stl_types::str::fmt::pointerify(C_CAST<u32>(url.flags)) + ")";
}
} // namespace Strings
} // namespace Coffee
