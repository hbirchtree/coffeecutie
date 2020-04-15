#pragma once

#include <peripherals/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace ld {

using namespace ::libc_types;
using namespace ::stl_types;

struct Version
{
    u32 major, minor, revision;
};

struct FunctionLoad_def
{
    using error_type = error_code;

    enum LoadFlags
    {
        NoFlags,
        GlobalSymbols,
    };

    struct Library;

    STATICINLINE
    /*!
     * \brief Load a dynamic library into memory
     * \param name Name of the library, eg. "test" for libtest.so
     * \param flags Options for library loading
     * \param ver Specify version of library
     * \return Pointer to the loaded library, platform-specific data
     */
    Library* GetLibrary(
        cstring, error_type&, LoadFlags = NoFlags, Version const* = nullptr)
    {
        return nullptr;
    }

    STATICINLINE
    cstring LinkError()
    {
        return nullptr;
    }

    STATICINLINE
    /*!
     * \brief Unload a loaded library, platform-dependent definition
     * \param lib
     * \return
     */
    bool UnloadLibrary(Library*, error_type&)
    {
        return false;
    }

    template<typename RType, typename... Args>
    struct Loader
    {
        using Fun = RType (*)(Args...);

        STATICINLINE
        /*!
         * \brief Load a function from an external library
         * \param lib Source library
         * \param funcName Name of function to load
         * \return
         */
        Fun GetFunction(Library*, cstring, error_type&)
        {
            return nullptr;
        }
    };
};

template<typename Obj, typename... Args>
struct ObjConstructor
{
    using return_type    = UqPtr<Obj>;
    using signature_type = return_type(Args...);

    /*! Needs to use a C-compatible signature,
     * and no type-safety guarantees across shared libraries.
     * If all fails, it will be a glorious train crash.
     */
    using LoaderFunction = Obj* (*)(Args...);

    LoaderFunction loader;

    inline bool valid() const
    {
        return loader;
    }

    inline return_type operator()(Args... args) const
    {
        auto ptr = loader(args...);
        return MkUqFrom<Obj>(ptr);
    }
};

template<typename FunctionLoader>
struct ObjectLoader_def
{
    template<typename Obj, typename... Args>
    STATICINLINE
        /*!
         * \brief Load an object constructor from a file
         * \param library Loaded library which should contain the function
         * \param constructor Name of constructor to load
         * \return
         */
        ObjConstructor<Obj, Args...>
        GetConstructor(
            typename FunctionLoader::Library*,
            cstring,
            typename FunctionLoader::error_type&)
    {
        return {};
    }
};

static const constexpr cstring DefaultConstructorFunction = "CoffeeLoader";

} // namespace ld
} // namespace platform
