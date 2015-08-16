#ifndef COFFEETEXTURE_H
#define COFFEETEXTURE_H

#include "general/common.h"
#include "general/data/coffeegameasset.h"
#include <QImage>

class CoffeeResource;
class CoffeeShader;

class CoffeeTexture : public QObject,public CoffeeGameAsset
{
    Q_PROPERTY(QObject* resource READ resource)
    Q_PROPERTY(QString textureFile READ textureFile)
    Q_PROPERTY(uint textureHandle READ getHandleOnly)
    Q_PROPERTY(bool validTexture READ isValidTexture)
    Q_PROPERTY(CoffeeGLTextureType type READ type WRITE setType)

    Q_OBJECT
public:
    enum CoffeeGLTextureType {
        None, //Means nothing

        Cubemap,CubemapDice, //Dice version uses a single image as source

        Texture1D,Texture2D,Texture3D
    };
    Q_ENUMS(CoffeeGLTextureType)

    enum CoffeeTextureType{
        CoffeeTexture_Diffusion,CoffeeTexture_Specular,
        CoffeeTexture_Highlight/*aka shininess*/,CoffeeTexture_Bumpmap,
        CoffeeTexture_Transparency,

        CoffeeTexture_Ambient,CoffeeTexture_Heightmap,
        CoffeeTexture_Emissive,CoffeeTexture_Displacement,

        CoffeeTexture_Lightmap,CoffeeTexture_Reflection,

        CoffeeTexture_Undefined
    };
    Q_ENUMS(CoffeeTextureType)

    CoffeeTexture(QObject *parent, QMap<GLenum, CoffeeResource *> mapping); //Creates a cubemap
    CoffeeTexture(QObject *parent, CoffeeResource* r, const QRect &targetArea); //Creates a cubemap from a single image file
    CoffeeTexture(QObject *parent, CoffeeResource *r);
    CoffeeTexture(QObject *parent,aiTexture* texture);

    CoffeeTexture(QObject* parent, GLuint handle);
    ~CoffeeTexture();

    bool isValidTexture();
    void load();
    void unload();

    void setProcessor(std::function<QImage(QImage input)> func);

    GLuint getHandle();
    GLuint getHandleOnly(); //because the other invokes gl* functions

    QString textureFile() const;

    QObject* resource();

    CoffeeGLTextureType type() const;

    GLenum getGlTextureType() const;

    void bindTexture(GLenum unit);

public slots:
    void setType(CoffeeGLTextureType type);

private:
    std::function<QImage(QImage input)> imageProcessor = [](QImage input){
        //We can use this to process the image the way we want.
        return input;
    };

    bool b_to_reload = false;

    bool validTexture = false;
    QImage texture;
    GLuint textureHandle = 0;
    GLenum textureType;

    QMap<GLenum,CoffeeResource*> cubemapping;

    QPointer<CoffeeResource> res;
    CoffeeGLTextureType m_type;
};

#endif // COFFEETEXTURE_H
