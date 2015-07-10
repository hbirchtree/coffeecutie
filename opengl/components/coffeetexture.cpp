#include "coffeetexture.h"

#include <QColor>
#include <QRgb>
#include "opengl/helpers/texturehelper.h"
#include "engine/data/coffeeresource.h"

CoffeeTexture::CoffeeTexture(QObject *parent, QMap<GLenum, CoffeeResource*> mapping) : QObject(parent)
{
    this->cubemapping = mapping;
    setType(Cubemap);
}

CoffeeTexture::CoffeeTexture(QObject *parent, CoffeeResource* r,
                             //TODO : Use this parameter
                             const QRect &targetArea) :
    QObject(parent)
{
    this->res = r;
    connect(res.data(),&CoffeeResource::resourceChanged,[=](){
        unloadTexture();
        loadTexture();
    });
    setType(CubemapDice);
}

CoffeeTexture::CoffeeTexture(QObject *parent, CoffeeResource* r) : QObject(parent)
{
    this->res = r;
    connect(res.data(),&CoffeeResource::resourceChanged,[=](){
        b_to_reload = true;
    });
    setType(Texture2D);
}

CoffeeTexture::CoffeeTexture(QObject *parent, aiTexture *texture) : QObject(parent)
{
    qWarning("Unimplemented constructor!");

    if(texture->mHeight==0){
        qDebug() << "Creating texture from format:" << texture->achFormatHint;
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
    return m_type==Cubemap||m_type==CubemapDice;
}

void CoffeeTexture::loadTexture()
{

    if(isAllocated()){
        addAllocation();
        return;
    }
    switch(type()){
    case CoffeeTexture::Cubemap:{
        if(cubemapping.size()!=6)
            qWarning("Invalid size of cube mapping!");
        QImage probe = QImage::fromData(*cubemapping.first()->data());
        QMap<GLenum,QImage> source;
        for(GLenum map : cubemapping.keys())
            source.insert(map,QImage::fromData(*cubemapping.value(map)->data()));
        textureHandle = TextureHelper::allocCubeTexture(GL_RGBA8,GL_BGRA,
                                                        probe.width(),probe.height(),
                                                        source,1,GL_UNSIGNED_BYTE);
        break;
    }
    case CoffeeTexture::CubemapDice:{
        QImage src = QImage::fromData(*res->data());
        if(src.isNull())
            qFatal("Failed to load cubemap dice: Invalid image");
        if((float)src.width()/(float)src.height()!=3.f/4.f)
            qWarning("Cubemap dice does not have correct dimensions!");
        int w_d = src.width()/4,h_d = src.height()/3;
        if(src.width()%w_d!=0||src.height()%h_d!=0||h_d!=w_d)
            qWarning("Cubemap dice not cut correctly!");
        /* This is the arrangement (Maybe?):
         *  ________
         * |  U    |
         * |W N E S|
         * |_ D____|
         *
        */
        QMap<GLenum,QImage> mapping;
        //Up, down
        mapping.insert(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,src.copy(w_d,0,w_d,h_d));
        mapping.insert(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,src.copy(w_d,h_d*2,w_d,h_d));
        //West, east
        mapping.insert(GL_TEXTURE_CUBE_MAP_POSITIVE_X,src.copy(0,h_d,w_d,h_d));
        mapping.insert(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,src.copy(w_d*2,h_d,w_d,h_d));
        //North, south
        mapping.insert(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,src.copy(w_d,h_d,w_d,h_d));
        mapping.insert(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,src.copy(w_d*3,h_d,w_d,h_d));
        textureHandle = TextureHelper::allocCubeTexture(GL_RGBA8,GL_BGRA,
                                                        w_d,h_d,
                                                        mapping,1,GL_UNSIGNED_BYTE);
        break;
    }
    case CoffeeTexture::Texture2D:{
        texture = QImage::fromData(*res->data());
        texture = imageProcessor(texture);
        textureHandle = TextureHelper::allocTexture(GL_RGBA8,GL_BGRA,
                                                    texture.width(),texture.height(),
                                                    texture.bits(),3,GL_UNSIGNED_BYTE);
        break;
    }
    case CoffeeTexture::Texture3D:
        //TODO : find a way to load 3D textures
        break;
    default:
        break;
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
    if(b_to_reload){
        unloadTexture();
        loadTexture();
        b_to_reload = false;
    }
    if(validTexture)
        return textureHandle;
    else
        return 0;
}

GLuint CoffeeTexture::getHandleOnly()
{
    return textureHandle;
}

QString CoffeeTexture::textureFile() const
{
    return res->source();
}

QObject *CoffeeTexture::resource()
{
    return res;
}

CoffeeTexture::CoffeeGLTextureType CoffeeTexture::type() const
{
    return m_type;
}

GLenum CoffeeTexture::getGlTextureType() const
{
    switch(type()){
    case CoffeeTexture::Cubemap:
    case CoffeeTexture::CubemapDice:
        return GL_TEXTURE_CUBE_MAP;
    case CoffeeTexture::Texture3D:
        return GL_TEXTURE_3D;
    default:
        return GL_TEXTURE_2D;
    }
}

void CoffeeTexture::setType(CoffeeTexture::CoffeeGLTextureType type)
{
    m_type = type;
}
