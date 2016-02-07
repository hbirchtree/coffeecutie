#include <coffee/graphics_apis/opengl/glbinding/wrappers/ctexture.h>

#include <coffee/graphics_apis/opengl/glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

size_t coffee_graphics_tex_get_size(
        const CTextureSize &size, const CTexFormat &fmt)
{
    size_t psize = 0;
    switch(fmt)
    {
    case CTexFormat::BGRA:
    case CTexFormat::RGBA:
    case CTexFormat::DepthStencil:
        psize = 4;
        break;
    case CTexFormat::RGB:
    case CTexFormat::Depth:
        psize = 3;
        break;
    case CTexFormat::RG:
        psize = 2;
        break;
    case CTexFormat::RED:
    case CTexFormat::BLUE:
    case CTexFormat::GREEN:
    case CTexFormat::Stencil:
        psize = 1;
        break;
    default:break;
    }
    return CMath::max(1,size.width)*CMath::max(1,size.height)*CMath::max(1,size.depth)*psize;
}

void coffee_graphics_tex_mipmap(CTexture &tex)
{
    glGenerateTextureMipmap(tex.handle);
}

void coffee_graphics_tex_mipmap_safe(CTexture &tex)
{
    coffee_graphics_bind(tex);
    glGenerateMipmap(gl_get(tex.type));
    coffee_graphics_unbind(tex);
}

void coffee_graphics_activate(CTexture &tex)
{
    coffee_graphics_bind(tex);
    coffee_graphics_unbind(tex);
}

void coffee_graphics_alloc(size_t count, CTexture *tex)
{
    glGenTextures(1,&tex->handle);
}

void coffee_graphics_free(size_t count, CTexture *tex)
{
    glDeleteTextures(1,&tex->handle);
}

void coffee_graphics_bind(CTexture const& tex)
{
    glBindTexture(gl_get(tex.type),tex.handle);
}

void coffee_graphics_unbind(CTexture const& tex)
{
    glBindTexture(gl_get(tex.type),0);
}

void coffee_graphics_tex_bind_image(
        const CTexture &tex, const CGuint &unit,
        const CGint &level, bool layered,
        const CGint &layer, const CTextureAccess &access)
{
    glBindImageTexture(unit,tex.handle,level,
                       layered ? GL_TRUE : GL_FALSE,
                       layer,gl_get(access),gl_get(tex.format));
}

CTexture::CTexture():
    type(CTexType::Tex2D),
    handle(0),
    levels(1),
    format(CTexIntFormat::None)
{
}

void coffee_graphics_tex_sparsify(CTexture &tex, bool enable)
{
    CGint v;
    if(enable)
        v = 1;
    else
        v = 0;

    glTextureParameteri(tex.handle,gl_get(CTexParam::SparseTexture),v);
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

void coffee_graphics_tex_memset_region(
        CTexture &tex, const CGint &level,
        const CTextureRegion &region,
        const CDataType &type, c_cptr data)
{
    glClearTexSubImage(
                tex.handle, level,
                region.x(),region.y(),region.z(),
                region.width,region.height,region.depth,
                gl_get(tex.format),
                gl_get(type),
                data);
}

void coffee_graphics_tex_dump(const CTexture &tex, cstring filename)
{
    CStbImageLib::CStbImage img;

    img.bpp = 4;
    img.size.w = tex.size.width;
    img.size.h = tex.size.height;

    coffee_graphics_tex_readtexels(
                tex,0,CTexFormat::RGBA,CDataType::UByte,
                img.data);


    CResources::CResource fl(filename);
    CStbImageLib::SavePNG(&fl,&img);
    FileCommit(fl);
    FileFree(fl);

    CFree(img.data);
}

CTextureData *coffee_graphics_tex_create_texdata(
        CResources::CResource const& resource, c_ptr location)
{
    CStbImageLib::CStbImage img;
    if(!CStbImageLib::LoadData(&img,&resource))
        return nullptr;

    CTextureData* ptr;
    if(location)
        ptr = new (location) CTextureData;
    else
        ptr = new CTextureData;

    ptr->data = img.data;
    ptr->size.width = img.size.w;
    ptr->size.height = img.size.h;

    switch(img.bpp)
    {
    case 4:
        ptr->format = CTexFormat::RGBA;
        break;
    case 3:
        ptr->format = CTexFormat::RGB;
        break;
    case 2:
        ptr->format = CTexFormat::RG;
        break;
    case 1:
        ptr->format = CTexFormat::RED;
        break;
    default:
        ptr->format = CTexFormat::RGBA;
    }

    ptr->datatype = CDataType::UByte;

    return ptr;
}

void coffee_graphics_tex_free_texdata(CTextureData *texd)
{
    CFree((void*)(*(&texd->data)));
    delete texd;
}

CImportedTexture coffee_graphics_tex_create_rtexdata(const CResources::CResource &resource)
{
    return CImportedTexture(coffee_graphics_tex_create_texdata(resource,nullptr));
}

}
}
