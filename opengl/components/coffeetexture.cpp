#include "coffeetexture.h"

#include <QColor>
#include <QRgb>
#include "opengl/helpers/texturehelper.h"

CoffeeTexture::CoffeeTexture(QObject *parent, QMap<GLenum, QString> mapping) : QObject(parent)
{
    this->cubemapping = mapping;
    this->b_cubemap = true;
}

CoffeeTexture::CoffeeTexture(QObject *parent, QString filename) : QObject(parent)
{
    m_textureFile = filename;
    texture = QImage(filename);
}

CoffeeTexture::CoffeeTexture(QObject *parent, QByteArray *img) : QObject(parent)
{
    texture = QImage::fromData(*img);
    delete img;
}

CoffeeTexture::CoffeeTexture(QObject *parent, aiTexture *texture) : QObject(parent)
{
    if(texture->mHeight==0){
        qDebug() << "Creating texture from format:" << texture->achFormatHint;
//        QImage::Format fmt;
        qDebug() << "Unimplemented function: decompressing texture data";
    }else{
        this->texture = QImage(QSize(texture->mWidth,texture->mHeight),QImage::Format_RGBA8888);
        for(uint x=0;x<texture->mWidth;x++)
            for(uint y=0;y<texture->mHeight;y++){
                aiTexel s = texture->pcData[y*texture->mWidth+x];
                QRgb c = qRgba(s.r,s.g,s.b,s.a);
                this->texture.setColor(y*texture->mWidth+x,c);
            }
    }
}

CoffeeTexture::~CoffeeTexture()
{
}

bool CoffeeTexture::isValidTexture()
{
    return validTexture;
}

bool CoffeeTexture::isCubemap()
{
    return b_cubemap;
}

void CoffeeTexture::loadTexture()
{
    if(isAllocated()){
        addAllocation();
        return;
    }
    if(isCubemap()){
        if(cubemapping.size()!=6)
            qWarning("Invalid size of cube mapping!");
        QImage probe(cubemapping.first());
        QMap<GLenum,QImage> source;
        for(GLenum map : cubemapping.keys())
            source.insert(map,QImage(cubemapping.value(map)));
        textureHandle = TextureHelper::allocCubeTexture(GL_RGBA8,GL_BGRA,
                                                        probe.width(),probe.height(),
                                                        source,1,GL_UNSIGNED_BYTE);
    }else{
        texture = imageProcessor(texture);
        textureHandle = TextureHelper::allocTexture(GL_RGBA8,GL_BGRA,
                                                    texture.width(),texture.height(),
                                                    texture.bits(),3,GL_UNSIGNED_BYTE);
    }
    if(textureHandle>0){
        addAllocation();
        validTexture = true;
    }else{
        qWarning("Failed to allocate texture!");
    }
}

void CoffeeTexture::unloadTexture()
{
    removeAllocation();
    if(isAllocated())
        return;
    glDeleteTextures(1,&textureHandle);
    textureHandle = 0;
    validTexture = false;
}

void CoffeeTexture::setProcessor(std::function<QImage (QImage)> func)
{
    this->imageProcessor = func;
}

GLuint CoffeeTexture::getHandle()
{
    if(validTexture)
        return textureHandle;
    else
        return 0;
}

QString CoffeeTexture::textureFile() const
{
    return m_textureFile;
}
