#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

bool CTextureTools::coffee_graphics_tex_2d_define(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    glTextureStorage2D(texture->handle,texture->levels,data->format,
                       data->lengths[0],data->lengths[1]);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_define(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    glTextureStorage3D(texture->handle,texture->levels,texture->format,
                       data->lengths[0],data->lengths[1],data->lengths[2]);
    return true;
}

bool CTextureTools::coffee_graphics_tex_cube_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, GLint level)
{
    glTextureSubImage3D(texture->handle,level,0,0,0,
                        data->lengths[0],data->lengths[1],data->lengths[2],
            texture->format,data->datatype,data->data);
    return true;
}

bool CTextureTools::coffee_graphics_tex_2d_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, GLint level)
{
    glTextureSubImage2D(texture->handle,level,0,0,
                        data->lengths[0],data->lengths[1],
            texture->format,data->datatype,data->data);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, GLint level)
{
    glTexImage3D(texture->textureType,level,static_cast<GLint>(data->format),
                 data->lengths[0],data->lengths[1],data->lengths[2],
            0,texture->format,data->datatype,data->data);
    return true;
}

GLuint64 coffee_graphics_tex_get_handle(CTexture *tex)
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

void coffee_graphics_tex_download_texture(
        const CTexture *tex, GLint level,
        szptr size, GLenum format,
        CStbImageLib::CStbImage *img)
{
    img->data = (ubyte*)malloc(size);
    glGetTextureImage(
                tex->handle,level,format,
                GL_UNSIGNED_BYTE,size,img->data);
}

void coffee_graphics_tex_use(
        const CTexture *tex)
{
    coffee_graphics_tex_make_resident(tex);
}

void coffee_graphics_tex_use_safe(
        const CTexture *tex)
{
    glBindMultiTextureEXT((GLenum)((uint32)(GL_TEXTURE0)+tex->unit),
                          tex->textureType,tex->handle);
}

void coffee_graphics_tex_unload(
        const CTexture *tex)
{
    coffee_graphics_tex_make_nonresident(tex);
}

void coffee_graphics_tex_unload_safe(
        const CTexture *tex)
{
    glBindMultiTextureEXT((GLenum)((uint32)(GL_TEXTURE0)+tex->unit),
                          tex->textureType,0);
}

void coffee_graphics_tex_param(
        const CTexture *tex, GLenum param, GLint val)
{
    glTextureParameteri(tex->handle,param,val);
}

void coffee_graphics_tex_param(
        const CTexture *tex, GLenum param, GLenum val)
{
    coffee_graphics_tex_param(tex,param,(GLint)val);
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
    glGenerateMipmap(tex->textureType);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_param_safe(
        const CTexture *tex, GLenum param, GLint val)
{
    coffee_graphics_bind(tex);
    glTexParameteri(tex->textureType,param,val);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_param_safe(
        const CTexture *tex, GLenum param, GLenum val)
{
    coffee_graphics_tex_param_safe(tex,param,(GLint)val);
}

bool CTextureTools::coffee_graphics_tex_2d_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data, GLint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage2D(texture->textureType,level,0,0,
                        data->lengths[0],data->lengths[1],
            texture->format,data->datatype,data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data,
        GLint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage3D(texture->textureType,level,0,0,0,
                        data->lengths[0],data->lengths[1],data->lengths[2],
            texture->format,data->datatype,data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_cube_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data,
        GLint level)
{
    coffee_graphics_tex_3d_store_safe(texture,data,level);
    return false;
}

bool CTextureTools::coffee_graphics_tex_2d_define_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    coffee_graphics_bind(texture);
    glTexStorage2D(texture->textureType,texture->levels,data->format,
                       data->lengths[0],data->lengths[1]);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_define_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    coffee_graphics_bind(texture);
    glTexStorage3D(texture->textureType,texture->levels,texture->format,
                       data->lengths[0],data->lengths[1],data->lengths[2]);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_define(
        const CTexture *tex, const CTextureTools::CTextureData *data)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_define(tex,data);
    case 3: return coffee_graphics_tex_3d_define(tex,data);
    }
    return false;
}

bool CTextureTools::coffee_graphics_tex_define_safe(
        const CTexture *tex, const CTextureTools::CTextureData *data)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_define_safe(tex,data);
    case 3: return coffee_graphics_tex_3d_define_safe(tex,data);
    }
    return false;
}

bool CTextureTools::coffee_graphics_tex_store(
        const CTexture *tex, const CTextureTools::CTextureData *data, GLint level)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_store(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store(tex,data,level);
    }
    return false;
}

bool CTextureTools::coffee_graphics_tex_store_safe(
        const CTexture *tex, const CTextureTools::CTextureData *data, GLint level)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_store_safe(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store_safe(tex,data,level);
    }
    return false;
}

void coffee_graphics_tex_activate(const CTexture *tex)
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
    glBindTexture(tex->textureType,tex->handle);
}

void coffee_graphics_unbind(const CTexture *tex)
{
    glBindTexture(tex->textureType,0);
}

void CTextureTools::coffee_graphics_tex_free_texdata(CTextureTools::CTextureData *tex)
{
    free(tex->lengths);
    tex->lengths = nullptr;
}

}
}
