#pragma once

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/coffee_resource.h>

namespace Coffee{

namespace CResources{
/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */

struct Resource;

}

struct Url;

struct Path
{
    Path(cstring path):
        internUrl(path ? path : "")
    {
    }
    Path(CString const& path):
        internUrl(path)
    {
    }
    Path():
        Path(nullptr)
    {
    }

    CString internUrl;

    Path removeExt() const;
    Path addExtension(cstring ext) const;
    Path fileBasename() const;

    CString extension() const;

    Path dirname();

    Path operator+(cstring component);
    FORCEDINLINE Path operator+(CString const& component)
    {
        return *this + component.c_str();
    }
    Path operator+(Path const& path);

    Path& operator=(Url const& url);

    STATICINLINE Path Mk(cstring p)
    {
        return {p};
    }
};

struct Url
{
    enum StorageType
    {
        Undefined,
        Local,
        Networked,
    };

    CString internUrl;
    StorageType category;
    ResourceAccess flags;
    HTTPAccess netflags;

    CString cachedUrl;

    FORCEDINLINE bool isLocal() const
    {
        return category == Local;
    }

    /*!
     * \brief Operator with const which does not perform caching of dereferenced URLs
     * \return
     */
    CString operator*() const;
    /*!
     * \brief Does the same as const operator*, except it caches the value for later dereferences. Currently not very useful since most `FileFun::*` functions do not take `Url&`, but rather `Url const&`.
     * \return
     */
    CString operator*();

    CResources::Resource rsc() const;

    Url operator+(Path const& path) const;

    FORCEDINLINE Url& operator+=(Path const& path)
    {
        *this = *this + path;
        return *this;
    }

    operator Path()
    {
        return {internUrl};
    }

private:
    CString DereferenceLocalPath() const;
};

FORCEDINLINE Url MkUrl(cstring urlString)
{
    return
    {
        urlString,
                Url::Local,
                ResourceAccess::SpecifyStorage|ResourceAccess::AssetFile,
                HTTPAccess::None,
        {}
    };
}

FORCEDINLINE Url MkUrl(cstring urlString, ResourceAccess access)
{
    return
    {
        urlString,
                Url::Local,
                access,
                HTTPAccess::None,
        {}
    };
}

FORCEDINLINE Url MkSysUrl(cstring urlString)
{
    return
    {
        urlString,
                Url::Local,
                ResourceAccess::SpecifyStorage|ResourceAccess::SystemFile,
                HTTPAccess::None,
        {}
    };
}

FORCEDINLINE Url operator "" _url(const char* url, size_t)
{
    return MkUrl(url);
}

namespace Strings{
extern CString to_string(Url const& url);
extern CString to_string(Path const& path);
}
}
