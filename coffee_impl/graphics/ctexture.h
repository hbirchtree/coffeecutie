#ifndef CTEXTURE
#define CTEXTURE

#include "glbinding.h"
#include "coffee.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CTexture
{
    GLenum          textureType     = GL_NONE;
    GLuint          handle          = 0;
    GLsizei         levels          = 1;
    GLenum          components      = GL_NONE;

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
    const byte*     data        = nullptr;
    szptr           size        = 0;
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
    if(sizeof...(sizelist)>0)
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
    _coffee_create_texturelist(val->lengths,sizes...);
}

static bool coffee_texture2d_store(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=2)
        return false;
    glTexImage2D(texture->textureType,texture->levels,static_cast<GLint>(texture->components),
                 data->lengths[0],data->lengths[1],
            0,data->format,data->datatype,data->data);
    return true;
}
static bool coffee_texture3d_store(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=3)
        return false;
    glTexImage3D(texture->textureType,texture->levels,static_cast<GLint>(texture->components),
                 data->lengths[0],data->lengths[1],data->lengths[2],
            0,data->format,data->datatype,data->data);
    return true;
}

static bool coffee_texture_cube_store(CTexture* texture, GLenum type, CTextureData* data)
{
    if(data->dimensions!=3)
        return false;
    glTexImage3D(type,texture->levels,static_cast<GLint>(texture->components),
                 data->lengths[0],data->lengths[1],data->lengths[2],
            0,data->format,data->datatype,data->data);
    return true;
}

static bool coffee_texture2d_define(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=2)
        return false;
    glTexStorage2D(texture->textureType,texture->levels,data->format,
                   data->lengths[0],data->lengths[1]);
    return true;
}
static bool coffee_texture3d_define(CTexture* texture, CTextureData* data)
{
    if(data->dimensions!=3)
        return false;
    glTexStorage3D(texture->textureType,texture->levels,data->format,
                   data->lengths[0],data->lengths[1],data->lengths[2]);
    return true;
}
}

}
}

#endif // CTEXTURE
