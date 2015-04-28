#ifndef TEXTUREHELPER_H
#define TEXTUREHELPER_H

#include "general/common.h"
#include "general/filehandler.h"

class TextureHelper
{
public:
    static GLuint allocTexture(GLint internalFormat,GLint colorFormat,int w,int h,unsigned char* source);
    static GLuint loadTexture(QString filesource);
};

#endif // TEXTUREHELPER_H
