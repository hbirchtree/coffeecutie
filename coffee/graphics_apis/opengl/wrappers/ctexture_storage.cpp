#include "ctexture_storage.h"

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{



bool coffee_graphics_tex_2d_define(
        const CTexture *texture)
{
    glTextureStorage2D(texture->handle,texture->levels,
                       gl_get(texture->format),
                       texture->size.w,texture->size.h);
    return true;
}

bool coffee_graphics_tex_3d_define(
        const CTexture *texture)
{
    glTextureStorage3D(texture->handle,texture->levels,
                       gl_get(texture->format),
                       texture->size.w,texture->size.h,texture->size.d);
    return true;
}

bool coffee_graphics_tex_cube_store(
        const CTexture *texture, const CTextureData *data, CGint level)
{
    glTextureSubImage3D(texture->handle,level,0,0,0,
                        data->size.w,data->size.h,data->size.d,
            gl_get(data->format),
            gl_get(data->datatype),
            data->data);
    return true;
}

bool coffee_graphics_tex_2d_store(
        const CTexture *texture, const CTextureData *data, CGint level)
{
    glTextureSubImage2D(texture->handle,level,0,0,
                        data->size.w,data->size.h,
            gl_get(data->format),
            gl_get(data->datatype),
            data->data);
    return true;
}

bool coffee_graphics_tex_3d_store(
        const CTexture *texture, const CTextureData *data, CGint level)
{
    glTexImage3D(gl_get(texture->textureType),
                 level,
                 (CGint)data->format,
                 data->size.w,data->size.h,data->size.d,
                 0,
                 gl_get(data->format),
                 gl_get(data->datatype),
                 data->data);
    return true;
}

bool coffee_graphics_tex_2d_store_safe(
        const CTexture *texture, const CTextureData *data, CGint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage2D(
                gl_get(texture->textureType),
                level,0,0,
                data->size.w,data->size.h,
                gl_get(texture->format),
                gl_get(data->datatype),
                data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool coffee_graphics_tex_3d_store_safe(
        const CTexture *texture, const CTextureData *data,
        CGint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage3D(gl_get(texture->textureType),
                    level,0,0,0,
                    data->size.w,data->size.h,data->size.d,
            gl_get(texture->format),
            gl_get(data->datatype),
            data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool coffee_graphics_tex_cube_store_safe(
        const CTexture *texture, const CTextureData *data,
        CGint level)
{
    coffee_graphics_tex_3d_store_safe(texture,data,level);
    return false;
}

bool coffee_graphics_tex_2d_define_safe(
        const CTexture *texture)
{
    coffee_graphics_bind(texture);
    glTexStorage2D(gl_get(texture->textureType),
                   texture->levels,
                   gl_get(texture->format),
                   texture->size.w,texture->size.h);
    coffee_graphics_unbind(texture);
    return true;
}

bool coffee_graphics_tex_3d_define_safe(
        const CTexture *texture)
{
    coffee_graphics_bind(texture);
    glTexStorage3D(gl_get(texture->textureType),
                   texture->levels,
                   gl_get(texture->format),
                   texture->size.w,texture->size.h,texture->size.d);
    coffee_graphics_unbind(texture);
    return true;
}

bool coffee_graphics_tex_define(
        const CTexture *tex)
{
    switch(tex->size.dimensions())
    {
    case 2: return coffee_graphics_tex_2d_define(tex);
    case 3: return coffee_graphics_tex_3d_define(tex);
    }
    return false;
}

bool coffee_graphics_tex_define_safe(
        const CTexture *tex)
{
    switch(tex->size.dimensions())
    {
    case 2: return coffee_graphics_tex_2d_define_safe(tex);
    case 3: return coffee_graphics_tex_3d_define_safe(tex);
    }
    return false;
}

bool coffee_graphics_tex_store(
        const CTexture *tex, const CTextureData *data, CGint level)
{
    switch(data->size.dimensions())
    {
    case 2: return coffee_graphics_tex_2d_store(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store(tex,data,level);
    }
    return false;
}

bool coffee_graphics_tex_store_safe(
        const CTexture *tex, const CTextureData *data, CGint level)
{
    switch(data->size.dimensions())
    {
    case 2: return coffee_graphics_tex_2d_store_safe(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store_safe(tex,data,level);
    }
    return false;
}

void coffee_graphics_tex_2d_define_mutable(const CTexture *texture, const CTextureData* data)
{
    coffee_graphics_bind(texture);

    glTexImage2D(gl_get(texture->textureType),
                 0,
                 (GLint)gl_get(texture->format),
                 data->size.w,data->size.h,
                 0,
                 gl_get(data->format),
                 gl_get(data->datatype),
                 data->data);

    coffee_graphics_unbind(texture);
}

void coffee_graphics_tex_readpixels(
        const CTexture &tex, const CGint &level,
        const CTexFormat &fmt,
        const CDataType &dType, c_ptr dPtr)
{
    glGetTextureImage(tex.handle,level,gl_get(fmt),gl_get(dType),0,dPtr);
}

void coffee_graphics_tex_readpixels_safe(
        const CTexture &tex, const CGint &level,
        const CTexFormat &fmt,
        const CDataType &dType, c_ptr dPtr)
{
    coffee_graphics_bind(&tex);

    glGetTexImage(gl_get(tex.textureType),level,gl_get(fmt),gl_get(dType),dPtr);

    coffee_graphics_unbind(&tex);
}

}
}
