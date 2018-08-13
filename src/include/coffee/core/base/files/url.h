#pragma once

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {

namespace CResources {
/*!
 * \brief Easy redirection of all resources in the application. :)
 * \param prefix
 * \return
 */

struct Resource;

} // namespace CResources

struct Url;

struct Path
{
    Path(cstring path) : internUrl(path ? path : "")
    {
    }
    Path(CString const& path) : internUrl(path)
    {
    }
    Path() : Path(nullptr)
    {
    }

    CString internUrl;

    Path         removeExt() const;
    Path         addExtension(cstring ext) const;
    FORCEDINLINE Path addExtension(CString const& ext) const
    {
        return addExtension(ext.c_str());
    }
    Path fileBasename() const;

    CString extension() const;

    Path dirname() const;

    Path canonical() const;

    Path         operator+(cstring component) const;
    FORCEDINLINE Path operator+(CString const& component) const
    {
        return *this + component.c_str();
    }
    Path operator+(Path const& path) const;

    Path& operator=(Url const& url);

    STATICINLINE Path Mk(cstring p)
    {
        return {p};
    }

    FORCEDINLINE bool operator<(Path const& other) const
    {
        return internUrl < other.internUrl;
    }

    FORCEDINLINE bool operator>(Path const& other) const
    {
        return internUrl > other.internUrl;
    }

    FORCEDINLINE bool operator==(Path const& other) const
    {
        return internUrl == other.internUrl;
    }

    FORCEDINLINE bool operator!=(Path const& other) const
    {
        return internUrl != other.internUrl;
    }
};

struct Url
{
    enum StorageType
    {
        Undefined,
        Local,
        Networked,
        Memory,
    };

    CString     internUrl;
    StorageType category;
    RSCA        flags;
    HTTPAccess  netflags;

    CString cachedUrl;

    FORCEDINLINE bool isLocal() const
    {
        return category == Local;
    }

    /*!
     * \brief Operator with const which does not perform caching of dereferenced
     * URLs \return
     */
    CString operator*() const;
    /*!
     * \brief Does the same as const operator*, except it caches the value for
     * later dereferences. Currently not very useful since most `FileFun::*`
     * functions do not take `Url&`, but rather `Url const&`. \return
     */
    CString operator*();

    Url operator+(Path const& path) const;

    FORCEDINLINE Url& operator+=(Path const& path)
    {
        *this = *this + path;
        return *this;
    }

    operator Path() const
    {
        return {internUrl};
    }

    template<typename Resource, typename... Args>
    Resource rsc(Args... a) const
    {
        return Resource(a..., *this);
    }

  private:
    CString DereferenceLocalPath() const;
};

struct UrlParse
{
    static UrlParse From(Url const& url);

    CString protocol()
    {
        return m_protocol;
    }
    CString host()
    {
        return m_host;
    }
    CString resource()
    {
        return m_resource;
    }
    u32 port()
    {
        return m_port;
    }

    bool valid() const
    {
        return m_protocol.size();
    }

  private:
    UrlParse()
    {
    }
    CString m_protocol;
    CString m_host;
    CString m_resource;
    u32     m_port;
};

FORCEDINLINE Url MkUrl(cstring urlString)
{
    return {urlString, Url::Local, RSCA::AssetFile, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkUrl(cstring urlString, RSCA access)
{
    return {urlString, Url::Local, access, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkSysUrl(cstring urlString)
{
    return {urlString, Url::Local, RSCA::SystemFile, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkUrl(Path p, RSCA access = RSCA::SystemFile)
{
    return {p.internUrl.c_str(), Url::Local, access, HTTPAccess::None, {}};
}

/* Basic Url constructors */

FORCEDINLINE Url operator"" _url(const char* url, size_t)
{
    return MkUrl(url);
}

FORCEDINLINE Url operator"" _web(const char* url, size_t)
{
    return {url, Url::Networked, RSCA::None, HTTPAccess::DefaultAccess, {}};
}

/*
 *
 * Location constructors
 *
 *
 */

FORCEDINLINE Url operator"" _tmp(const char* url, size_t)
{
    return MkUrl(url, RSCA::TempFile);
}

FORCEDINLINE Url operator"" _cache(const char* url, size_t)
{
    return MkUrl(url, RSCA::CachedFile);
}

namespace Strings {
extern CString to_string(Url const& url);
extern CString to_string(Path const& path);
} // namespace Strings
} // namespace Coffee
