#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/enum/http_access.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/types.h>

namespace platform::url {

using namespace ::libc_types;
using namespace ::stl_types;
using namespace ::semantic;

struct Url;

struct Path
{
    explicit Path(cstring path) : internUrl(path ? path : "")
    {
    }
    explicit Path(CString const& path) : internUrl(path)
    {
    }
    Path() : Path(nullptr)
    {
    }

    STATICINLINE Path Mk(cstring p)
    {
        return Path(p);
    }

    CString internUrl;

    Path              removeExt() const;
    Path              addExtension(cstring ext) const;
    FORCEDINLINE Path addExtension(CString const& ext) const
    {
        return addExtension(ext.c_str());
    }
    Path fileBasename() const;

    CString extension() const;

    Path dirname() const;

    Vector<Path> components() const;

    Path              operator+(cstring component) const;
    FORCEDINLINE Path operator+(CString const& component) const
    {
        return *this + component.c_str();
    }
    Path operator+(Path const& path) const;

    FORCEDINLINE Path operator/(CString const& component) const
    {
        return *this + component;
    }
    FORCEDINLINE Path operator/(Path const& path) const
    {
        return *this + path;
    }

    Path& operator=(Url const& url);

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

    FORCEDINLINE bool operator<=(Path const& other) const
    {
        return internUrl <= other.internUrl;
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

    CString              internUrl{};
    StorageType          category{Local};
    semantic::RSCA       flags{RSCA::None};
    semantic::HTTPAccess netflags{HTTPAccess::None};

    CString cachedUrl{};

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
     * functions do not take `Url&`, but rather `Url const&`.
     * \return
     */
    CString operator*();

    Url operator+(Path const& path) const;

    FORCEDINLINE Url operator/(Path const& path) const
    {
        return *this + path;
    }

    FORCEDINLINE Url operator/(String const& path) const
    {
        return *this + Path(path);
    }

    FORCEDINLINE Url& operator+=(Path const& path)
    {
        *this = *this + path;
        return *this;
    }

    operator Path() const
    {
        return Path(internUrl);
    }

    explicit operator const char*() const
    {
        return internUrl.c_str();
    }

    explicit operator String() const
    {
        return internUrl;
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

namespace constructors {

FORCEDINLINE Url MkInvalidUrl()
{
    return {{}, Url::Undefined, RSCA::None, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkUrl(CString const& p, RSCA access = RSCA::SystemFile)
{
    return {p, Url::Local, access, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkSysUrl(CString const& urlString)
{
    return {urlString, Url::Local, RSCA::SystemFile, HTTPAccess::None, {}};
}

FORCEDINLINE Url MkSysUrl(Path const& urlString)
{
    return {
        urlString.internUrl,
        Url::Local,
        RSCA::SystemFile,
        HTTPAccess::None,
        {}};
}

FORCEDINLINE Url MkUrl(Path const& p, RSCA access = RSCA::SystemFile)
{
    return {p.internUrl.c_str(), Url::Local, access, HTTPAccess::None, {}};
}

/* Basic Url constructors */

FORCEDINLINE Url operator"" _url(const char* url, size_t)
{
    return MkUrl(url);
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

FORCEDINLINE Url operator"" _config(const char* url, size_t)
{
    return MkUrl(url, RSCA::ConfigFile);
}

FORCEDINLINE Url operator"" _sys(const char* url, size_t)
{
    return MkUrl(url, RSCA::SystemFile);
}

} // namespace constructors
} // namespace platform::url
