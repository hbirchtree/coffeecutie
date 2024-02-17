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
    explicit Path(std::string_view const& path)
        : internUrl(path)
    {
    }

    explicit Path()
    {
    }

    std::string internUrl;

    Path removeExt() const;
    Path addExtension(std::string_view const& ext) const;
    Path fileBasename() const;

    std::string extension() const;

    Path dirname() const;

    std::vector<Path> components() const;

    Path operator/(std::string_view const& component) const;
    Path operator/(Path const& path) const;

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

    Url url(RSCA flags = RSCA::SystemFile) const;
    Url url(HTTPAccess flags) const;
};

struct Url
{
    enum StorageType
    {
        Undefined,
        Local,
        Networked,
    };

    std::string          internUrl{};
    StorageType          category{Undefined};
    semantic::RSCA       flags{RSCA::None};
    semantic::HTTPAccess netflags{HTTPAccess::None};

    FORCEDINLINE bool isLocal() const
    {
        return category == Local;
    }

    FORCEDINLINE bool valid() const
    {
        return category != Undefined;
    }

    /*!
     * \brief Operator with const which does not perform caching of dereferenced
     * URLs \return
     */
    std::string operator*() const;

    Url operator+(Path const& path) const;

    FORCEDINLINE Url operator/(Path const& path) const
    {
        return *this + path;
    }

    FORCEDINLINE Url operator/(std::string const& path) const
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

    explicit operator std::string() const
    {
        return internUrl;
    }

    template<typename Resource, typename... Args>
    Resource rsc(Args... a) const
    {
        return Resource(a..., *this);
    }

    Path path() const
    {
        return Path{internUrl};
    }

  private:
    std::string DereferenceLocalPath() const;
};

inline bool operator<(Url const& u1, Url const& u2)
{
    return u1.internUrl < u2.internUrl;
}

struct UrlParse
{
    static UrlParse const from(Url const& url);

    std::string protocol;
    std::string host;
    std::string resource;
    u16         port{0};

    bool valid() const
    {
        return protocol.size();
    }
};

namespace constructors {

FORCEDINLINE Url MkUrl(std::string const& p, RSCA access = RSCA::SystemFile)
{
    return {
        .internUrl = p,
        .category  = Url::Local,
        .flags     = access,
        .netflags  = HTTPAccess::None,
    };
}

FORCEDINLINE Url MkSysUrl(std::string const& urlString)
{
    return {
        .internUrl = urlString,
        .category  = Url::Local,
        .flags     = RSCA::SystemFile,
        .netflags  = HTTPAccess::None,
    };
}

/*
 *
 * Location constructors
 *
 *
 */

FORCEDINLINE Url operator"" _asset(const char* url, size_t)
{
    return MkUrl(url, RSCA::AssetFile);
}

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

FORCEDINLINE Url invalid_url()
{
    return {.category = Url::Undefined};
}

} // namespace platform::url
