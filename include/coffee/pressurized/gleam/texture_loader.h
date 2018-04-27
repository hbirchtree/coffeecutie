#pragma once

#include <coffee/interfaces/file_resolver.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/edef/pixenum.h>

#include <coffee/graphics/apis/CGLeamRHI>

namespace Coffee{
namespace RHI{
namespace GLEAM{

template<typename Resource>
STATICINLINE UrlResolver TextureResolver(
        ResourceResolver<Resource> const& rr
        )
{
    UrlResolver::SingleResolver sres = [&](Url const& url)
    {
        static constexpr szptr num_formats = 5;

        static constexpr Array<Pair<PixFmt, cstring>, num_formats> formats =
        {{
            {PixFmt::S3TC, "dxt5"},
            {PixFmt::S3TC, "dxt3"},
            {PixFmt::S3TC, "dxt1"},
            {PixFmt::ETC1, "etc2"},
            {PixFmt::ETC1, "etc1"},
        }};

        Path baseName = url;
        baseName = baseName.removeExt();

        for(auto i : Range<>(num_formats))
        {
            auto const& fmt = formats[i];
            if(GLEAMAPI::TextureFormatSupport(fmt.first))
                if(rr.resolveResource())
                    return MkUrl(baseName.addExtension(fmt.second));
        }

        return url;
    };

    return {
        sres, UrlResolver::DefaultMulti
    };
}

}
}
}
