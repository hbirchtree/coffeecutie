#ifndef COFFEETEXTURE_H
#define COFFEETEXTURE_H

#include "general/common.h"
#include "opengl/helpers/texturehelper.h"
#include "general/data/coffeegameasset.h"
class CoffeeTexture : public QObject, public CoffeeGameAsset
{
    Q_OBJECT
public:

    enum DefaultTextureEnum{
        Texture_Diffusion,Texture_Specular,Texture_Highlight,Texture_Transparency,
        Texture_Bumpmap
    };

    CoffeeTexture(QObject *parent,QString filename);
    CoffeeTexture(QObject *parent,QByteArray* img);
    ~CoffeeTexture();

    void loadTexture();
    void unloadTexture();

    void setProcessor(std::function<QImage(QImage input)> func);

    GLint getHandle();

signals:

private:
    std::function<QImage(QImage input)> imageProcessor = [](QImage input){
        //We can use this to process the image the way we want.
        return input;
    };
    bool validTexture = false;
    QImage texture;
    GLint textureHandle = -1;
};

#endif // COFFEETEXTURE_H
