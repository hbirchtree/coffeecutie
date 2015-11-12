#include "ctexture.h"

#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

bool CTextureTools::coffee_graphics_tex_2d_define(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    glTextureStorage2D(texture->handle,texture->levels,
                       CG_GET(data->format,ctexint_map),
                       data->lengths[0],data->lengths[1]);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_define(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    glTextureStorage3D(texture->handle,texture->levels,
                       CG_GET(texture->format,ctexfmt_map),
                       data->lengths[0],data->lengths[1],data->lengths[2]);
    return true;
}

bool CTextureTools::coffee_graphics_tex_cube_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, CGint level)
{
    glTextureSubImage3D(texture->handle,level,0,0,0,
                        data->lengths[0],data->lengths[1],data->lengths[2],
            CG_GET(texture->format,ctexfmt_map),
            CG_GET(data->datatype,cdtypes_map),
            data->data);
    return true;
}

bool CTextureTools::coffee_graphics_tex_2d_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, CGint level)
{
    glTextureSubImage2D(texture->handle,level,0,0,
                        data->lengths[0],data->lengths[1],
            CG_GET(texture->format,ctexfmt_map),
            CG_GET(data->datatype,cdtypes_map),
            data->data);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_store(
        const CTexture *texture, const CTextureTools::CTextureData *data, CGint level)
{
    glTexImage3D(CG_GET(texture->textureType,ctextp_map),
                 level,
                 (CGint)data->format,
                 data->lengths[0],data->lengths[1],data->lengths[2],
            0,
            CG_GET(texture->format,ctexfmt_map),
            CG_GET(data->datatype,cdtypes_map),
            data->data);
    return true;
}

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

void coffee_graphics_tex_download_texture(const CTexture *tex, CGint level,
        CGsize size, CTexFormat format,
        CStbImageLib::CStbImage *img)
{
    img->data = (ubyte*)malloc(size);
    glGetTextureImage(
                tex->handle,
                level,
                CG_GET(format,ctexfmt_map),
                GL_UNSIGNED_BYTE,
                size,img->data);
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
                        CG_GET(param,ctexparm_map),
                        val);
}

void coffee_graphics_tex_param(const CTexture *tex, CTexParam param, CTexParamOpt val)
{
    coffee_graphics_tex_param(tex,param,(CGint)CG_GET(val,ctexparmopt_map));
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
    glGenerateMipmap(CG_GET(tex->textureType,ctextp_map));
    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_param_safe(const CTexture *tex, CTexParam param, CGint val)
{
    coffee_graphics_bind(tex);
    glTexParameteri(CG_GET(tex->textureType,ctextp_map),
                    CG_GET(param,ctexparm_map),
                    val);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_tex_param_safe(const CTexture *tex, CTexParam param, CTexParamOpt val)
{
    coffee_graphics_tex_param_safe(tex,param,(CGint)(CG_GET(val,ctexparmopt_map)));
}

bool CTextureTools::coffee_graphics_tex_2d_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data, CGint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage2D(
                CG_GET(texture->textureType,ctextp_map),
                level,0,0,
                data->lengths[0],data->lengths[1],
            CG_GET(texture->format,ctexfmt_map),
            CG_GET(data->datatype,cdtypes_map),
            data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data,
        CGint level)
{
    coffee_graphics_bind(texture);
    glTexSubImage3D(CG_GET(texture->textureType,ctextp_map),
                    level,0,0,0,
                    data->lengths[0],data->lengths[1],data->lengths[2],
            CG_GET(texture->format,ctexfmt_map),
            CG_GET(data->datatype,cdtypes_map),
            data->data);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_cube_store_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data,
        CGint level)
{
    coffee_graphics_tex_3d_store_safe(texture,data,level);
    return false;
}

bool CTextureTools::coffee_graphics_tex_2d_define_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    coffee_graphics_bind(texture);
    glTexStorage2D(CG_GET(texture->textureType,ctextp_map),
                   texture->levels,
                   CG_GET(texture->format,ctexfmt_map),
                       data->lengths[0],data->lengths[1]);
    coffee_graphics_unbind(texture);
    return true;
}

bool CTextureTools::coffee_graphics_tex_3d_define_safe(
        const CTexture *texture, const CTextureTools::CTextureData *data)
{
    coffee_graphics_bind(texture);
    glTexStorage3D(CG_GET(texture->textureType,ctextp_map),
                   texture->levels,
                   CG_GET(texture->format,ctexfmt_map),
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
        const CTexture *tex, const CTextureTools::CTextureData *data, CGint level)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_store(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store(tex,data,level);
    }
    return false;
}

bool CTextureTools::coffee_graphics_tex_store_safe(const CTexture *tex, const CTextureTools::CTextureData *data, CGint level)
{
    switch(data->dimensions)
    {
    case 2: return coffee_graphics_tex_2d_store_safe(tex,data,level);
    case 3: return coffee_graphics_tex_3d_store_safe(tex,data,level);
    }
    return false;
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
    glBindTexture(CG_GET(tex->textureType,ctextp_map),tex->handle);
}

void coffee_graphics_unbind(const CTexture *tex)
{
    glBindTexture(CG_GET(tex->textureType,ctextp_map),0);
}

void CTextureTools::coffee_graphics_tex_free_texdata(CTextureTools::CTextureData *tex)
{
    free(tex->lengths);
    tex->lengths = nullptr;
}

void coffee_graphics_tex_dump(const CTexture *tex, cstring filename)
{
    CStbImageLib::CStbImage img;

    img.bpp = 4;
    img.size.w = 1280;
    img.size.h = 720;

    coffee_graphics_tex_download_texture(
                tex,0,img.bpp*img.size.w*img.size.h,
                tex->format,&img);


    CResources::CResource fl(filename);
    CStbImageLib::coffee_stb_image_save_png(&fl,&img);
    coffee_file_commit(&fl);
    coffee_file_free(&fl);

    free(img.data);
}

void coffee_graphics_tex_paramf(const CTexture *tex, CTexParam param, scalar val)
{
    glTextureParameterf(
                tex->handle,
                CG_GET(param,ctexparm_map),
                val);
}

void coffee_graphics_tex_paramf_safe(const CTexture *tex, CTexParam param, scalar val)
{

    coffee_graphics_bind(tex);
    glTexParameterf(CG_GET(tex->textureType,ctextp_map),
                    CG_GET(param,ctexparm_map),
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
    format(CTexFormat::None),
    unit(-1),
    bhandle(0)
{
}

}
}
