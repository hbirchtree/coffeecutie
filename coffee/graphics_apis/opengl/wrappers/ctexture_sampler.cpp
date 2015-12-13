#include "ctexture_sampler.h"

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_alloc(size_t count, CTextureSampler *sampler)
{
    CGuint *handles = new CGuint[count];
    glGenSamplers(count,handles);
    for(size_t i=0;i<count;i++)
    {
        sampler[i].handle = handles[i];
        glBindSampler(0,sampler[i].handle);
    }
    glBindSampler(0,0);
    delete[] handles;
}

void coffee_graphics_free(size_t count, CTextureSampler *sampler)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = sampler[i].handle;
        sampler[i].handle = 0;
    }
    glDeleteSamplers(count,handles);
    delete[] handles;
}

void coffee_graphics_tex_parami(
        CTextureSampler &tex, CTexParam const& param, CGint const& val)
{
    glSamplerParameteri(tex.handle,
                        gl_get(param),
                        val);
}

void coffee_graphics_tex_paramf(
        CTextureSampler &tex, CTexParam const& param, scalar const& val)
{
    glSamplerParameterf(
                tex.handle,
                gl_get(param),
                val);
}

void coffee_graphics_tex_param(
        CTextureSampler &tex, CTexParam const& param, CTexParamOpt const& val)
{
    coffee_graphics_tex_parami(tex,param,(CGint)gl_get(val));
}

CGhnd64 coffee_graphics_tex_get_handle(CTexture const& texture,CTextureSampler &samp)
{
    samp.bhandle = glGetTextureSamplerHandleARB(texture.handle,samp.handle);
    return samp.bhandle;
}

void coffee_graphics_tex_make_resident(const CTextureSampler &samp)
{
    glMakeTextureHandleResidentARB(samp.bhandle);
}

void coffee_graphics_tex_make_nonresident(const CTextureSampler &samp)
{
    glMakeTextureHandleNonResidentARB(samp.bhandle);
}

void coffee_graphics_tex_load(CTextureSampler const& tex, const CTexture &)
{
    coffee_graphics_tex_make_resident(tex);
}

void coffee_graphics_tex_unload(CTextureSampler const& tex, const CTexture &)
{
    coffee_graphics_tex_make_nonresident(tex);
}

void coffee_graphics_tex_load_safe(CTextureSampler const& sampler, const CTexture &tex)
{
    glActiveTexture(GL_TEXTURE0+sampler.unit);
    glBindTexture(gl_get(tex.textureType),tex.handle);
    glBindSampler(sampler.unit,sampler.handle);
}

void coffee_graphics_tex_unload_safe(CTextureSampler const&, CTexture const&)
{
}

}
}
