#pragma once
#include <coffee/core/CFiles>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/core/type_safety.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee{

/*! \brief
 *
 * Extending some APIs, it becomes necessary to select between multiple file extensions.
 * An example of this is image types, where on some systems DXTn is supported,
 *  others support ETC1, and some support ASTC.
 *
 * The #resolve function may transform a Coffee::Url by replacing
 *  the file extension and other properties. 
 *
 * An example of this usage is in RHI::GLEAM::TextureResolver(),
 *  which tests for hardware support for different compressed texture formats,
 *  and yields the Url for the best supported format.
 * At the same time it looks up mipmap sizes such that textures with
 *  excessively high resolutions are not loaded on weaker hardware.
 *
 * The #multiplexResolve function is used for cases where a basename is used
 *  to refer to multiple files, and an extension is to be appended. 
 * It is more of a convenience function.
 *
 * An example of its usage is in the case where a model "bob" has the resources
 *  "bob.tex" and "bob.mesh" associated with it. An API may be fed a UrlResolver
 *  and the basename "bob" and will be able to locate "bob.tex" and "bob.mesh"
 *  on its own, with much fewer inputs to the function.
 *
 */
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

/*! \brief
 *
 * An API for translating a Url into a Bytes object autonomously.
 * This hides the use of ByteResolver objects such that the code only
 *  works with Url instances, not Url and an associated *::Resource type.
 *
 * An example where this is relevant is when creating VirtFS::Resource
 *  instances, which require a reference to the parent VirtFS::VFS upon construction.
 *
 * The #extResolver function provided is a convenience function around
 *  UrlResolver::multiplexResolve and #resolver.
 *
 */
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
