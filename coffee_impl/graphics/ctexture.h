#ifndef CTEXTURE
#define CTEXTURE

#include "glbinding.h"
#include "coffee.h"
#include "coffee_impl/image/cimage.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CTexture
{
    GLenum          textureType     = GL_NONE;
    GLuint          handle          = 0;
    GLsizei         levels          = 1;
    GLenum          format          = GL_NONE;

    void create(){
        glGenTextures(1,&handle);
    }
    void free(){
        glDeleteTextures(1,&handle);
        handle = 0;
    }
    void bind(){
        glBindTexture(textureType,handle);
    }
    void unbind(){
        glBindTexture(textureType,0);
    }

    void mipmap(){
        glGenerateMipmap(textureType);
    }
    void parameter(GLenum type, GLenum parm, GLint value){
        glTexParameteri(type,parm,value);
    }
    void parameter(GLenum parm, GLint value){
        parameter(textureType,parm,value);
    }
    void parameter(GLenum parm, GLenum value){
        parameter(parm,static_cast<GLint>(value));
    }
};

static GLuint64 coffee_tex_get_handle(CTexture* tex)
{
    return glGetTextureHandleARB(tex->handle);
}

static void coffee_tex_make_resident(GLuint64 handle)
{
    glMakeTextureHandleResidentARB(handle);
}

static void coffee_tex_make_nonresident(GLuint64 handle)
{
    glMakeTextureHandleNonResidentARB(handle);
}

static void coffee_tex_download_texture(
        CTexture* tex, GLint level,
        szptr size, GLenum format, CStbImageLib::CStbImage* img)
{
    img->data = (ubyte*)malloc(size);
    glGetTextureImage(
                tex->handle,level,format,
                GL_UNSIGNED_BYTE,size,img->data);
}

namespace CTextureTools{
struct CTextureFormat
{
    byte red        = 0;
    byte green      = 0;
    byte blue       = 0;
    byte alpha      = 0;

    byte depth      = 0;
    byte stencil    = 0;

    byte extra      = 0;
};
struct CTextureData
{
    const void*     data        = nullptr;
    byte            dimensions  = 0;
    szptr*          lengths     = nullptr;
    GLenum          format      = GL_NONE; //This is the format of the image
    GLenum          datatype    = GL_NONE; //This is the data type, most likely ubyte
    void freeData(){ //Important to free the length array
        free(lengths);
    }
};

template<typename arg, typename... sizes>
static void _coffee_create_texturelist(szptr* list, int index, arg subject)
{
    list[index] = subject;
}
template<typename arg, typename... sizes>
static void _coffee_create_texturelist(szptr* list, int index, arg subject, sizes... sizelist)
{
    list[index] = subject;
    _coffee_create_texturelist(list,index+1,sizelist...);
}

template<typename... Sizes>
static void coffee_create_texturesize(CTextureData* val, Sizes... sizes)
{
    const int n = sizeof...(sizes);
    if(n<1)
        return;
    val->dimensions = n;
    val->lengths = reinterpret_cast<szptr*>(malloc(sizeof(szptr)*n));
    _coffee_create_texturelist(val->lengths,0,sizes...);
}

static bool coffee_texture2d_store(CTexture* texture, CTextureData* data, GLint level)
{
    if(data->dimensions!=2)
        return false;
    glTextureSubImage2D(texture->handle,level,0,0,
                 data->lengths[0],data->lengths[1],
            texture->format,data->datatype,data->data);
    return true;
}
static bool coffee_texture3d_store(CTexture* texture, CTextureData* data, GLint level)
{
    if(data->dimensions!=3)
        return false;
    glTexImage3D(texture->textureType,level,static_cast<GLint>(data->format),
                 data->lengths[0],data->lengths[1],data->lengths[2],
            0,texture->format,data->datatype,data->data);
    return true;
}

static bool coffee_texture_cube_store(CTexture* texture, GLenum type, CTextureData* data, GLint level)
{
    if(data->dimensions!=3)
        return false;
    glTextureSubImage3D(texture->handle,level,0,0,0,
                 data->lengths[0],data->lengths[1],data->lengths[2],
            texture->format,data->datatype,data->data);
    return true;
}

static bool coffee_texture2d_define(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=2)
        return false;
    glTextureStorage2D(texture->handle,texture->levels,data->format,
                   data->lengths[0],data->lengths[1]);
    return true;
}
static bool coffee_texture3d_define(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=3)
        return false;
    glTextureStorage3D(texture->handle,texture->levels,texture->format,
                   data->lengths[0],data->lengths[1],data->lengths[2]);
    return true;
}
}

}
}

#endif // CTEXTURE
