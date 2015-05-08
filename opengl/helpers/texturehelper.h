#ifndef TEXTUREHELPER_H
#define TEXTUREHELPER_H

#include "general/common.h"
#include "general/filehandler.h"

class TextureHelper
{
public:
    static GLuint allocTexture(GLenum internalFormat, GLenum colorFormat, int w, int h, unsigned char *source, uint mipmaps, GLenum datatype);
    static GLuint loadTexture(QString filesource);
};

#endif // TEXTUREHELPER_H
