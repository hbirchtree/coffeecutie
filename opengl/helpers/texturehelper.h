#ifndef TEXTUREHELPER_H
#define TEXTUREHELPER_H

#include <QString>
#include <QImage>
#include "general/filehandler.h"
#include <GL/glew.h>
#include "glext.h"
#include "glcorearb.h"
#include "GLFW/glfw3.h"

class TextureHelper
{
    static GLint allocTexture(GLint internalFormat,GLint colorFormat,int w,int h,unsigned char* source){
        GLint handle;
        glGenTextures(1,&handle);
        glBindTexture(GL_TEXTURE_2D,handle);
        glTexImage2D(GL_TEXTURE_2D,0,internalFormat,w,h,0,colorFormat,GL_UNSIGNED_BYTE,source);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        return handle;
    }
    static GLint loadTexture(QString filesource){
        QImage texture(filesource);
        return allocTexture(GL_RGBA,GL_RGBA,texture.width(),texture.height(),texture.bits());
    }
};

#endif // TEXTUREHELPER_H
