#include <wrappers/ctexture_storage.h>
#include <glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_defimmutable_1d(CTexture &tex)
{
    glTextureStorage1D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.w);
}

void coffee_graphics_tex_defimmutable_2d(CTexture &tex)
{
    glTextureStorage2D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.w,tex.size.h);
}

void coffee_graphics_tex_defimmutable_3d(CTexture &tex)
{
    glTextureStorage3D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.w,tex.size.h,tex.size.d);
}

void coffee_graphics_tex_substore_1d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage1D(tex.handle,level,
                        reg.x(),
                        reg.w,
                        gl_get(data.format),gl_get(data.datatype),
                        data.data);
}

void coffee_graphics_tex_substore_2d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage2D(tex.handle,level,
                        reg.x(),reg.y(),
                        reg.w,reg.h,
                        gl_get(data.format),gl_get(data.datatype),
                        data.data);
}

void coffee_graphics_tex_substore_3d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage3D(tex.handle,level,
                        reg.x(),reg.y(),reg.z(),
                        reg.w,reg.h,reg.d,
                        gl_get(data.format),gl_get(data.datatype),
                        data.data);
}

void coffee_graphics_tex_readtexelregion(
        const CTexture& tex, const CTextureRegion& reg,
        const CGint& level, const CTexFormat& fmt,
        const CDataType& dType, c_ptr dPtr)
{
    glGetTextureSubImage(tex.handle,level,
                         reg.x(),reg.y(),reg.z(),
                         reg.w,reg.h,reg.d,
                         gl_get(fmt),gl_get(dType),
                         0, dPtr);
}

}
}
