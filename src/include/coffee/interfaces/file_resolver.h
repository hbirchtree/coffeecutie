#pragma once
#include <coffee/core/CFiles>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/core/type_safety.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee{

struct UrlResolver
{
    using SingleResolver = Function<Url(Url const&)>;
    using MultiResolver = Function<Url(Url const&, CString const&)>;

    /*!
     * \brief Basic resolver for changing extensions and etc.
     */
    SingleResolver resolve;
    /*!
     * \brief Resolver for appending different extensions
     */
    MultiResolver multiplexResolve;

    static Url DefaultSingle(Url const& url)
    {
        return url;
    }

    static Url DefaultMulti(Url const& path, CString const& ext)
    {
        Path newPath = Path(path).addExtension(ext.c_str());
        Url newUrl = MkUrl(newPath, path.flags);

        return newUrl;
    }

    static UrlResolver DefaultResolver()
    {
        return {
            DefaultSingle, DefaultMulti
        };
    }
};

template<typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
struct ResourceResolver
{
    using Resolver = Function<Resource(Url const&)>;
    using FileQuery = Function<bool(Path const&, Vector<Url>&)>;

    Resolver resolveResource;
    FileQuery resourceQuery;
};

struct BytesResolver
{
    using Resolver = Function<Bytes(Url const&)>;
    using ExtResolver = Function<Bytes(Url const&, CString const&)>;

    Resolver resolver;
    ExtResolver extResolver;

    template<typename UResolver, typename RResolver>
    static BytesResolver Create(UResolver const& ur,
                                RResolver const& rr)
    {
        return {
            [rr, ur](Url const& url)
            {
                return rr.resolveResource(ur.resolve(url));
            },
            [rr, ur](Url const& url, CString const& ext)
            {
                return rr.resolveResource(
                            ur.multiplexResolve(url, ext)
                            );
            }
        };
    }
};

}
