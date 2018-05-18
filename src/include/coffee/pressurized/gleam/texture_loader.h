#pragma once

#include <coffee/core/string_casting.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/interfaces/file_resolver.h>

#include <coffee/graphics/apis/CGLeamRHI>

namespace Coffee {
namespace RHI {
namespace GLEAM {

template<typename Resource>
STATICINLINE UrlResolver TextureResolver(
    BytesResolver const& br, ResourceResolver<Resource> const& rr, u32 mipsize)
{
    UrlResolver::SingleResolver sres = [&](Url const& url) {
        static constexpr szptr num_formats = 5;

        static const Array<Pair<PixFmt, cstring>, num_formats> formats = {{
            {PixFmt::S3TC, "dxt5"},
            {PixFmt::S3TC, "dxt3"},
            {PixFmt::S3TC, "dxt1"},
            {PixFmt::ETC1, "etc2"},
            {PixFmt::ETC1, "etc1"},
        }};

        auto mip_ext = cast_pod(mipsize);

        Path baseName = url;
        baseName      = baseName.removeExt();

        Vector<Url> urls;
        rr.resourceQuery(baseName, urls);

        for(auto i : Range<>(num_formats))
        {
            auto const& fmt = formats[i];
            if(GLEAMAPI::TextureFormatSupport(fmt.first))
            {
                auto rscUrl = MkUrl(baseName.addExtension(mip_ext.c_str())
                                        .addExtension(fmt.second));
                auto rsc    = br.resolver(rscUrl);
                if(rsc.data)
                    return rscUrl;
            }
        }

        Path outPath = url;
        auto origExt = outPath.extension();
        outPath =
            outPath.addExtension(mip_ext.c_str()).addExtension(origExt.c_str());

        return url;
    };

    return {sres, UrlResolver::DefaultMulti};
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
