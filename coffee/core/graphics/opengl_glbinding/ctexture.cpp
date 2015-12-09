#include "ctexture.h"

#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

CGuint64 coffee_graphics_tex_get_handle(CTexture *tex)
{
    tex->bhandle = glGetTextureHandleARB(tex->handle);
    return tex->bhandle;
}

void coffee_graphics_tex_make_resident(const CTexture* tex)
{
    glMakeTextureHandleResidentARB(tex->bhandle);
}

void coffee_graphics_tex_make_nonresident(const CTexture* tex)
{
    glMakeTextureHandleNonResidentARB(tex->bhandle);
}

void coffee_graphics_tex_use(
        const CTexture *tex)
{
    coffee_graphics_tex_make_resident(tex);
}

void coffee_graphics_tex_use_safe(
        const CTexture *tex)
{
    glBindTextures(tex->unit,1,&tex->handle);
}

void coffee_graphics_tex_unload(
        const CTexture *tex)
{
    coffee_graphics_tex_make_nonresident(tex);
}

void coffee_graphics_tex_unload_safe(
        const CTexture*)
{
}

void coffee_graphics_tex_param(
        const CTexture *tex, CTexParam param, CGint val)
{
    glTextureParameteri(tex->handle,
                        gl_get(param),
                        val);
}

void coffee_graphics_tex_param(const CTexture *tex, CTexParam param, CTexParamOpt val)
{
    coffee_graphics_tex_param(tex,param,(CGint)gl_get(val));
}

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

void coffee_graphics_tex_param_safe(const CTexture *tex, CTexParam param, CGint val)
{
    coffee_graphics_bind(tex);
    glTexParameteri(gl_get(tex->textureType),
                    gl_get(param),
                    val);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_param_safe(const CTexture *tex, CTexParam param, CTexParamOpt val)
{
    coffee_graphics_tex_param_safe(tex,param,(CGint)(gl_get(val)));
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

void coffee_graphics_tex_paramf(const CTexture *tex, CTexParam param, scalar val)
{
    glTextureParameterf(
                tex->handle,
                gl_get(param),
                val);
}

void coffee_graphics_tex_paramf_safe(const CTexture *tex, CTexParam param, scalar val)
{

    coffee_graphics_bind(tex);
    glTexParameterf(gl_get(tex->textureType),
                    gl_get(param),
                    val);
    coffee_graphics_unbind(tex);
}

CTextureFunctionBinds::CTextureFunctionBinds():
    load(nullptr),
    unload(nullptr),
    define(nullptr),
    store(nullptr),
    param_e(nullptr),
    param(nullptr),
    paramf(nullptr),
    mipmap(nullptr)
{
}

CTexture::CTexture():
    textureType(CTexType::Tex2D),
    handle(0),
    levels(1),
    format(CTexIntFormat::None),
    unit(-1),
    bhandle(0)
{
}

void coffee_graphics_tex_sparsify(CTexture *tex, bool enable)
{
    CGint v;
    if(enable)
        v = 1;
    else
        v = 0;

    coffee_graphics_tex_param(tex,CTexParam::SparseTexture,v);
}

}
}
