#pragma once

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{

namespace CResources{
/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */
extern void FileResourcePrefix(cstring prefix);

struct Resource;

}

struct Path
{
    CString internUrl;

    Path removeExt();
    Path addExtension(cstring ext);
    Path fileBasename();

    Path operator+(cstring component);
    FORCEDINLINE Path operator+(CString const& component)
    {
        return *this + component.c_str();
    }
    Path operator+(Path const& path);
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

    FORCEDINLINE bool isLocal() const
    {
        return category == Local;
    }

    CString operator*() const;

    CResources::Resource rsc() const;

    Url& operator+(Path const& path);

    FORCEDINLINE Url& operator+=(Path const& path)
    {
        return *this + path;
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
                HTTPAccess::None
    };
}

FORCEDINLINE Url MkUrl(cstring urlString, ResourceAccess access)
{
    return
    {
        urlString,
                Url::Local,
                access,
                HTTPAccess::None
    };
}

FORCEDINLINE Url MkSysUrl(cstring urlString)
{
    return
    {
        urlString,
                Url::Local,
                ResourceAccess::SpecifyStorage|ResourceAccess::SystemFile,
                HTTPAccess::None
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
