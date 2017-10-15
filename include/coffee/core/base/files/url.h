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

struct Url
{
    enum StorageType
    {
        Undefined,
        Local,
        Networked,
    };

    cstring internUrl;
    StorageType category;
    ResourceAccess flags;
    HTTPAccess netflags;

    FORCEDINLINE bool isLocal() const
    {
        return category == Local;
    }

    CString operator*() const;

    CResources::Resource rsc() const;

private:
    CString DereferenceLocalPath() const;
};

FORCEDINLINE Url MkUrl(cstring urlString)
{
    return {urlString, Url::Local,
                ResourceAccess::SpecifyStorage
                |ResourceAccess::AssetFile,
                HTTPAccess::None};
}

FORCEDINLINE Url MkUrl(cstring urlString, ResourceAccess access)
{
    return {urlString, Url::Local, access, HTTPAccess::None};
}

FORCEDINLINE Url operator "" _url(const char* url, size_t)
{
    return MkUrl(url);
}

}
