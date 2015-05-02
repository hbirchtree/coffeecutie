#include "coffeetexture.h"

CoffeeTexture::CoffeeTexture(QObject *parent, QString filename) : QObject(parent)
{
    texture = QImage(filename);
}

CoffeeTexture::~CoffeeTexture()
{
}

void CoffeeTexture::loadTexture()
{
//    if(texture.isNull())
//        return;
    texture = imageProcessor(texture);
    QImage renderable = texture.convertToFormat(QImage::Format_ARGB32);
    textureHandle = TextureHelper::allocTexture(static_cast<int>(GL_RGBA),GL_RGBA,texture.width(),texture.height(),texture.bits());
    if(textureHandle>=0)
        validTexture = true;
}

void CoffeeTexture::unloadTexture()
{
    GLuint txt[1];
    txt[0] = textureHandle;
    glDeleteTextures(1,txt);
    textureHandle = 0;
    validTexture = false;
}

void CoffeeTexture::setProcessor(std::function<QImage (QImage)> func)
{
    this->imageProcessor = func;
}

GLint CoffeeTexture::getHandle()
{
    if(validTexture)
        return textureHandle;
    else
        return 0;
}
