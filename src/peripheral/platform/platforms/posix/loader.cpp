#include <platforms/posix/loader.h>

#include <peripherals/stl/string_casting.h>

namespace platform {
namespace ld {
namespace posix {

const char* linking_error_category::name() const noexcept
{
    return "linking_error_code";
}

std::string linking_error_category::message(int error_code) const
{
    switch(error_code)
    {
    case 0:
        return "No Error";
    default:
        return "General Error";
    }
}

UqPtr<FunctionLoader::Library> FunctionLoader::GetLibrary(
    cstring                   libName,
    error_type&               ec,
    FunctionLoader::LoadFlags flags,
    const Version*            ver)
{
    CString perm[4];
#if defined(COFFEE_LINUX)
    perm[0] = "lib" + CString(libName) + shared_object_extension;
#elif defined(COFFEE_APPLE)
    perm[0] = "lib" + CString(libName) + shared_object_extension;
#else
    perm[0] = "lib" + CString(libName) + shared_object_extension;
#endif
    if(ver)
    {
#if defined(COFFEE_LINUX)
        perm[1] = perm[0] + "." + cast_pod(ver->major);
        perm[2] = perm[1] + "." + cast_pod(ver->minor);
        perm[3] = perm[2] + "." + cast_pod(ver->revision);
#elif defined(COFFEE_APPLE)
        perm[1] = perm[0] + cast_pod(ver->major);
        perm[2] = perm[1] + cast_pod(ver->minor);
        perm[3] = perm[2] + cast_pod(ver->revision) + shared_object_extension;

        perm[0] = perm[0] + shared_object_extension;
        perm[1] = perm[1] + shared_object_extension;
        perm[2] = perm[2] + shared_object_extension;
#endif
    }

    szptr i      = 0;
    void* handle = nullptr;

    while(!handle && i < 4)
    {
        handle = dlopen(perm[i].c_str(), RTLD_NOW | flags);
        if(ver)
            i++;
        else
            break;
    }

    if(!handle)
    {
        ec    = 1;
        ec    = LinkError();
        errno = 0;

        return nullptr;
    } else
    {
        auto lib    = MkUq<Library>();
        lib->handle = handle;
        errno       = 0;
        return lib;
    }
}

bool FunctionLoader::UnloadLibrary(UqPtr<Library>&& lib, error_type& ec)
{
    bool stat = dlclose(lib->handle) == 0;

    if(!stat)
        ec = LinkError();

    return stat;
}

} // namespace posix
} // namespace ld
} // namespace platform
