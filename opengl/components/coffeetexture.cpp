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
    addActiveUser();
    if(textureHandle>0){
        qDebug() << "texture is already loaded";
        return;
    }
//    if(texture.isNull())
//        return;
    texture = imageProcessor(texture);
    textureHandle = TextureHelper::allocTexture(static_cast<int>(GL_RGBA),GL_BGRA,texture.width(),texture.height(),texture.bits());
    if(textureHandle>0)
        validTexture = true;
}

void CoffeeTexture::unloadTexture()
{
    removeActiveUser();
    if(getActiveUses()>1){
        qDebug() << "resource in use elsewhere; keeping it loaded";
        return;
    }
    GLuint txt[1];
    txt[0] = textureHandle;
    glDeleteTextures(1,txt);
    textureHandle = 0;
    validTexture = false;
    removeActiveUser();
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
