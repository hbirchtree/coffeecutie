#include "texturehelper.h"

GLuint TextureHelper::allocTexture(GLint internalFormat, GLenum colorFormat, int w, int h, unsigned char *source){
    GLuint handle;
    glGenTextures(1,&handle);
    glBindTexture(GL_TEXTURE_2D,handle);
    glTexImage2D(GL_TEXTURE_2D,0,internalFormat,w,h,0,colorFormat,GL_UNSIGNED_BYTE,(GLvoid*)source);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,static_cast<int>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,static_cast<int>(GL_LINEAR_MIPMAP_LINEAR));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,static_cast<int>(GL_LINEAR_MIPMAP_LINEAR));

    glBindTexture(GL_TEXTURE_2D,0);
    return handle;
}

GLuint TextureHelper::loadTexture(QString filesource){
    QImage texture(filesource);
    //Mess with the image here if you want to
    if(texture.isNull())
        return -1;
    return allocTexture(static_cast<int>(GL_RGBA),GL_RGBA,texture.width(),texture.height(),texture.bits());
}
