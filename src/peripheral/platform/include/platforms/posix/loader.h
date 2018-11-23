#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE) || defined(COFFEE_ANDROID)

#include <platforms/base/loader.h>

/*dlopen,dlsym,dlerror,dlclose*/
#include <dlfcn.h>

namespace platform {
namespace ld {
namespace posix {

struct linking_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

struct FunctionLoader : FunctionLoad_def
{
    using error_type = domain_error_code<int, linking_error_category>;

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    static const constexpr cstring shared_object_extension = ".so";
#elif defined(COFFEE_APPLE)
    static const constexpr cstring shared_object_extension = ".dylib";
#endif

    STATICINLINE cstring LinkError()
    {
        return dlerror();
    }

    enum LoadFlags
    {
        NoFlags,
        GlobalSymbols = RTLD_GLOBAL,
    };

    struct Library
    {
        void* handle;
    };

    static UqPtr<Library> GetLibrary(
        cstring        libName,
        error_type&    ec,
        LoadFlags      flags = NoFlags,
        Version const* ver   = nullptr);

    static bool UnloadLibrary(UqPtr<Library>&& lib, error_type& ec);

    template<typename RType, typename... Args>
    struct Loader : FunctionLoad_def::Loader<RType, Args...>
    {
        using Fun = RType (*)(Args...);

        STATICINLINE
        Fun GetFunction(UqPtr<Library>& lib, cstring funcName, error_type& ec)
        {
            void* sym = dlsym(lib->handle, funcName);
            if(!sym)
            {
                ec = LinkError();
                return nullptr;
            } else
                return C_RCAST<Fun>(sym);
        }
    };
};

struct ObjectLoader : ObjectLoader_def<FunctionLoader>
{
    using Loader = FunctionLoader;

    template<typename Obj, typename... Args>
    STATICINLINE ObjConstructor<Obj> GetConstructor(
        UqPtr<Loader::Library>& library,
        cstring                 constructor,
        Loader::error_type&     ec)
    {
        return {Loader::Loader<Obj*, Args...>::GetFunction(
            library, constructor, ec)};
    }
};

} // namespace posix

using FunctionLoader = posix::FunctionLoader;
using ObjectLoader   = posix::ObjectLoader;

} // namespace ld
} // namespace platform
#endif
