#include "ctexture.h"

#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_tex_mipmap(
        const CTexture *tex)
{
    glGenerateTextureMipmap(tex->handle);
}

void coffee_graphics_tex_mipmap_safe(
        const CTexture *tex)
{
    coffee_graphics_bind(tex);
    glGenerateMipmap(gl_get(tex->textureType));
    coffee_graphics_unbind(tex);
}

void coffee_graphics_activate(const CTexture *tex)
{
    coffee_graphics_bind(tex);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_alloc(CTexture *tex)
{
    glGenTextures(1,&tex->handle);
}

void coffee_graphics_free(CTexture *tex)
{
    glDeleteTextures(1,&tex->handle);
}

void coffee_graphics_bind(const CTexture *tex)
{
    glBindTexture(gl_get(tex->textureType),tex->handle);
}

void coffee_graphics_unbind(const CTexture *tex)
{
    glBindTexture(gl_get(tex->textureType),0);
}

CTextureFunctionBinds::CTextureFunctionBinds():
    load(nullptr),
    unload(nullptr),
    define(nullptr),
    store(nullptr),
    mipmap(nullptr)
{
}

CTexture::CTexture():
    textureType(CTexType::Tex2D),
    handle(0),
    levels(1),
    format(CTexIntFormat::None)
{
}

void coffee_graphics_tex_sparsify(CTexture *tex, bool enable)
{
    CGint v;
    if(enable)
        v = 1;
    else
        v = 0;

    glTextureParameteri(tex->handle,gl_get(CTexParam::SparseTexture),v);
}

void coffee_graphics_tex_memset(
        CTexture &tex, CGint const& level,
        CDataType const& type, c_cptr data)
{
    glClearTexImage(
                tex.handle,level,
                gl_get(tex.format),
                gl_get(type),
                data);
}

void coffee_graphics_tex_memset_region(CTexture &tex, const CGint &level, const CTextureRegion &region, const CDataType &type, c_cptr data)
{
    glClearTexSubImage(
                tex.handle, level,
                region.x(),region.y(),region.z(),
                region.w,region.h,region.d,
                gl_get(tex.format),
                gl_get(type),
                data);
}

}
}
