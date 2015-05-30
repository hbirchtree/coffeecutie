#ifndef COFFEETEXTURE_H
#define COFFEETEXTURE_H

#include "general/common.h"
#include "opengl/helpers/texturehelper.h"
#include "general/data/coffeegameasset.h"
class CoffeeTexture : public QObject, public CoffeeGameAsset
{
    Q_PROPERTY(QString textureFile READ textureFile)
    Q_PROPERTY(uint textureHandle READ getHandle)
    Q_PROPERTY(bool validTexture READ isValidTexture)
    Q_PROPERTY(bool cubemap READ isCubemap)

    Q_OBJECT
public:

    enum DefaultTextureEnum{
        Texture_Diffusion,Texture_Specular,Texture_Highlight,Texture_Transparency,
        Texture_Bumpmap
    };

    CoffeeTexture(QObject *parent,QMap<GLenum,QString> mapping); //Creates a cubemap
    CoffeeTexture(QObject *parent,QString filename);
    CoffeeTexture(QObject *parent,QByteArray* img);
    ~CoffeeTexture();

    bool isValidTexture();
    bool isCubemap();
    void loadTexture();
    void unloadTexture();

    void setProcessor(std::function<QImage(QImage input)> func);

    GLuint getHandle();

    QString textureFile() const;

signals:

private:
    std::function<QImage(QImage input)> imageProcessor = [](QImage input){
        //We can use this to process the image the way we want.
        return input;
    };
    bool b_cubemap = false;
    bool validTexture = false;
    QImage texture;
    GLuint textureHandle = 0;

    QMap<GLenum,QString> cubemapping;

    QString m_textureFile;
};

#endif // COFFEETEXTURE_H
