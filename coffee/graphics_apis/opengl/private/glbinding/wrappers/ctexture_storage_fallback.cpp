#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture_storage_fallback.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_defmutable_1d(CTexture &tex)
{
    coffee_graphics_bind(tex);

    glTexImage1D(gl_get(tex.type),tex.levels,(GLint)gl_get(tex.format),
                 tex.size.width,
                 0,GL_NONE,GL_NONE,nullptr);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_defmutable_2d(CTexture &tex)
{
    coffee_graphics_bind(tex);

    glTexImage2D(gl_get(tex.type),tex.levels,(GLint)gl_get(tex.format),
                 tex.size.width,tex.size.height,
                 0,GL_NONE,GL_NONE,nullptr);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_defmutable_3d(CTexture &tex)
{
    coffee_graphics_bind(tex);

    glTexImage3D(gl_get(tex.type),tex.levels,(GLint)gl_get(tex.format),
                 tex.size.width,tex.size.height,tex.size.depth,
                 0,GL_NONE,GL_NONE,nullptr);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_substore_1d_safe(
        CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    coffee_graphics_bind(tex);

    glTexSubImage1D(gl_get(tex.type),
                    level,
                    reg.x(),reg.width,
                    gl_get(data.format),
                    gl_get(data.datatype),
                    data.data);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_substore_2d_safe(
        CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    coffee_graphics_bind(tex);

    glTexSubImage2D(gl_get(tex.type),
                    level,
                    reg.x(),reg.y(),
                    reg.width,reg.height,
                    gl_get(data.format),
                    gl_get(data.datatype),
                    data.data);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_substore_3d_safe(
        CTexture &tex, const CTextureData &data, const CGint &level, const CTextureRegion &reg)
{
    coffee_graphics_bind(tex);

    glTexSubImage3D(gl_get(tex.type),
                    level,
                    reg.x(),reg.y(),reg.z(),
                    reg.width,reg.height,reg.depth,
                    gl_get(data.format),
                    gl_get(data.datatype),
                    data.data);

    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_sparse_commit_safe(CTexture &tex, bool commit, const CGint &level, const CTextureRegion &reg)
{
    coffee_graphics_bind(tex);
    glTexPageCommitmentARB(gl_get(tex.type),level,
                           reg.x(),reg.y(),reg.z(),
                           reg.width,reg.height,reg.depth,
                           (commit) ? GL_TRUE : GL_FALSE);
    coffee_graphics_unbind(tex);
}

}
}
