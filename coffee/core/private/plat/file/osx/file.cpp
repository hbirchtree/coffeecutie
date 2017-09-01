#include <coffee/core/plat/file/osx/file.h>

#include <coffee/core/plat/plat_environment.h>
#include "../file_abstraction.h"
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>

namespace Coffee{
namespace CResources{
namespace Mac{

#if defined(COFFEE_APPLE_MOBILE)
static CString GetApplicationHome()
{
    char* home = getenv("HOME");
    if(!home)
        RUNOUTTHEWINDOW();
    return home;
}
#endif

CString MacFileFun::NativePath(cstring fn)
{
    if(fn[0] == '/')
        return fn;

    if(AssetApi::GetAsset(fn))
    {
        CFBundleRef bun = CFBundleGetMainBundle();
        if(!bun)
            return fn;
        CFURLRef path = CFBundleCopyBundleURL(bun);
        CFStringRef pathstr = CFURLCopyFileSystemPath(path,kCFURLPOSIXPathStyle);
        CFStringEncoding enc = CFStringGetSystemEncoding();
        const char* pathcstr = CFStringGetCStringPtr(pathstr,enc);
        CString out = pathcstr;
        CFRelease(pathstr);
        CFRelease(path);
#if !defined(COFFEE_APPLE_MOBILE)
        /* On OSX, the resource structure is different */
        out = Env::ConcatPath(out.c_str(),"Contents/Resources");
#endif
        out = Env::ConcatPath(out.c_str(),AssetApi::GetAsset(fn));
        //fprintf(stderr,"Filename: %s\n",out.c_str());
        return out;
    }else
        return fn;
}

CString MacFileFun::NativePath(cstring fn, ResourceAccess storage)
{
#if defined(COFFEE_APPLE_MOBILE)
    const CString appHome = GetApplicationHome();
    if((storage & (ResourceAccess::ConfigFile
                |ResourceAccess::TemporaryFile
                |ResourceAccess::CachedFile))
            != ResourceAccess::None)
    {
        CString p = {};
        if(feval(storage, ResourceAccess::ConfigFile))
            p = Env::ConcatPath(appHome.c_str(), "Documents");
        else if(feval(storage, ResourceAccess::TemporaryFile))
            p = Env::ConcatPath(appHome.c_str(), "tmp");
        else if(feval(storage, ResourceAccess::CachedFile))
            p = Env::ConcatPath(appHome.c_str(), "Library/Caches");
        fn = AssetApi::GetAsset(fn);
        return Env::ConcatPath(p.c_str(), fn);
    }
#else
    if(feval(storage,ResourceAccess::TemporaryFile))
        return Env::ConcatPath("/tmp",fn);
#endif
    return NativePath(fn);
}

}
}
}
