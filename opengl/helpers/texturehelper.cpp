#include "texturehelper.h"

GLuint TextureHelper::allocTexture(GLenum internalFormat,
                                   GLenum colorFormat,
                                   int w, int h,
                                   unsigned char *source,
                                   uint mipmaps,
                                   GLenum datatype){
    if(mipmaps<1)
        qWarning("Texture defined with no mipmaps!");
    GLuint handle;
    glGenTextures(1,&handle);
    glBindTexture(GL_TEXTURE_2D,handle);
    glTexStorage2D(GL_TEXTURE_2D,mipmaps,internalFormat,w,h);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,colorFormat,datatype,source);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,static_cast<int>(GL_LINEAR_MIPMAP_LINEAR));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,static_cast<int>(GL_LINEAR_MIPMAP_LINEAR));

    glBindTexture(GL_TEXTURE_2D,0);
    return handle;
}

GLuint TextureHelper::allocCubeTexture(GLenum internalFormat,
                                       GLenum colorFormat,
                                       int w, int h,
                                       QMap<GLenum,QImage> source,
                                       uint mipmaps,
                                       GLenum datatype)
{
    GLuint cubemap;
    glGenTextures(1,&cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubemap);

    glTexStorage2D(GL_TEXTURE_CUBE_MAP,1,internalFormat,w,h);
    for(GLenum map : source.keys()){
        glTexSubImage2D(map,0,0,0,w,h,colorFormat,datatype,source.value(map).bits());
//        glGenerateMipmap(map);
    }

    //mipmap filtering is *bad* for this
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
                    GL_TEXTURE_MAG_FILTER,static_cast<int>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
                    GL_TEXTURE_MIN_FILTER,static_cast<int>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
                    GL_TEXTURE_WRAP_S,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
                    GL_TEXTURE_WRAP_R,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
                    GL_TEXTURE_WRAP_T,static_cast<int>(GL_CLAMP_TO_EDGE));

    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    return cubemap;
}

GLuint TextureHelper::loadTexture(QString filesource){
    QImage texture(filesource);
    //Mess with the image here if you want to
    if(texture.isNull())
        return -1;
    return allocTexture(GL_RGBA8,GL_RGBA8,texture.width(),texture.height(),texture.bits(),3,GL_UNSIGNED_BYTE);
}
