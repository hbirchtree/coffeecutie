#pragma once
#include <coffee/core/CFiles>
#include <coffee/interfaces/byte_provider.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

namespace Coffee {

/*! \brief
 *
 * Extending some APIs, it becomes necessary to select between multiple file
 * extensions. An example of this is image types, where on some systems DXTn is
 * supported, others support ETC1, and some support ASTC.
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
    using MultiResolver  = Function<Url(Url const&, std::string const&)>;

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

    static Url DefaultMulti(Url const& path, std::string const& ext)
    {
        using namespace platform::url;

        Path newPath = Path(path).addExtension(ext.c_str());
        Url  newUrl  = newPath.url(path.flags);

        return newUrl;
    }

    static UrlResolver DefaultResolver()
    {
        return {DefaultSingle, DefaultMulti};
    }
};

template<typename Resource>
struct ResourceResolver
{
    using Resolver  = std::function<Resource(Url const&)>;
    using FileQuery = std::function<bool(Path const&, std::vector<Url>&)>;

    Resolver  resolveResource;
    FileQuery resourceQuery;
};

/*! \brief
 *
 * An API for translating a Url into a Bytes object autonomously.
 * This hides the use of ByteResolver objects such that the code only
 *  works with Url instances, not Url and an associated *::Resource type.
 *
 * An example where this is relevant is when creating VirtFS::Resource
 *  instances, which require a reference to the parent VirtFS::VFS upon
 * construction.
 *
 * The #extResolver function provided is a convenience function around
 *  UrlResolver::multiplexResolve and #resolver.
 *
 */
struct BytesResolver
{
    using Resolver    = Function<Bytes(Url const&)>;
    using ExtResolver = Function<Bytes(Url const&, std::string const&)>;

    Resolver    resolver;
    ExtResolver extResolver;

    template<typename UResolver, typename RResolver>
    static BytesResolver Create(UResolver const& ur, RResolver const& rr)
    {
        return {
            [rr, ur](Url const& url) {
                return rr.resolveResource(ur.resolve(url));
            },
            [rr, ur](Url const& url, std::string const& ext) {
                return rr.resolveResource(ur.multiplexResolve(url, ext));
            }};
    }
};

template<typename ResolverType>
struct ResolverPair
{
    ResolverType& resolver;
    Url const&    url;
};

} // namespace Coffee
