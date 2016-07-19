#include <coffee/core/plat/file/osx/file.h>

#include <coffee/core/plat/plat_environment.h>
#include "../file_abstraction.h"
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>

namespace Coffee{
namespace CResources{
namespace Mac{

CString MacFileFun::NativePath(cstring fn)
{
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
#if defined(COFFEE_APPLE_MOBILE)
        /* On OSX, the resource structure is different */
        out = Env::ConcatPath(out.c_str(),"Contents/Resources");
#endif
        out = Env::ConcatPath(out.c_str(),AssetApi::GetAsset(fn));
        fprintf(stderr,"Filename: %s\n",out.c_str());
        return out;
    }else
        return fn;
}

}
}
}
