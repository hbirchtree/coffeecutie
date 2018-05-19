#pragma once

#include <coffee/interfaces/file_resolver.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_rhi.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

STATICINLINE UrlResolver ShaderResolver(APILevel level)
{
    UrlResolver::SingleResolver sres = [=](Url const& urlPath)
    {
        Path path = urlPath;

        CString api_name = "core";
        CString ext = path.extension();
        CString glsl_version = cast_pod(
                    GLSLVersionFromAPI(level)
                    );

        if(GLEAMAPI::LevelIsOfClass(level, APIClass::GLES))
            api_name = "es";

        return MkUrl(

                    path
                    .removeExt()
                    .addExtension(api_name.c_str())
                    .addExtension(glsl_version.c_str())
                    .addExtension(ext.c_str()),

                    urlPath.flags
                    );
    };

    return {
        sres, UrlResolver::DefaultMulti
    };
}

}
}
}
