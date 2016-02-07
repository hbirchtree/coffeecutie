#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture_storage.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_defimmutable_1d(CTexture &tex)
{
    glTextureStorage1D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.width);
}

void coffee_graphics_tex_defimmutable_2d(CTexture &tex)
{
    glTextureStorage2D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.width,tex.size.height);
}

void coffee_graphics_tex_defimmutable_3d(CTexture &tex)
{
    glTextureStorage3D(tex.handle,tex.levels,
                       gl_get(tex.format),
                       tex.size.width,tex.size.height,tex.size.depth);
}

void coffee_graphics_tex_substore_1d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage1D(tex.handle,level,
                        reg.x(),
                        reg.width,
                        gl_get(data.format),gl_get(data.datatype),
                        data.data);
}

void coffee_graphics_tex_substore_2d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage2D(tex.handle,level,
                        reg.x(),reg.y(),
                        reg.width,reg.height,
                        gl_get(data.format),gl_get(data.datatype),
                        data.data);
}

void coffee_graphics_tex_substore_3d(CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    glTextureSubImage3D(tex.handle,level,
                        reg.x(),reg.y(),reg.z(),
                        reg.width,reg.height,reg.depth,
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
                         reg.width,reg.height,reg.depth,
                         gl_get(fmt),gl_get(dType),
                         0, dPtr);
}

void coffee_graphics_tex_sparse_commit(
        CTexture &tex, bool commit, const CGint &level, const CTextureRegion &reg)
{
    glTexturePageCommitmentEXT(tex.handle,level,
                               reg.x(),reg.y(),reg.z(),
                               reg.width,reg.height,reg.depth,
                               (commit) ? GL_TRUE : GL_FALSE);
}

}
}
