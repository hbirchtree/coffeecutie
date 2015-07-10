#ifndef COFFEEOUTPUTSURFACE_H
#define COFFEEOUTPUTSURFACE_H

#include "coffeeobject.h"

class CoffeeFrameBufferObject;
class ShaderContainer;
class CoffeeMesh;
class CoffeeWorldOpts;
class TextureMapping;
class ShaderMapping;
class ShaderVariant;
class CoffeeTexture;

class CoffeeOutputChannel //Mapping of framebuffer textures to uniforms
{
public:
    CoffeeOutputChannel(const QString &uniform,int textureIndex,int textureUnit);
    QString uniformName;
    int textureIndex;
    int textureUnit;
};

class CoffeeOutputSurface : public QObject,public CoffeeObject
{
    Q_PROPERTY(QObject* shader READ getShader WRITE setShader)
    Q_PROPERTY(QObject* framebuffer READ framebufferQObject WRITE setFramebuffer)

    Q_OBJECT

public:
    CoffeeOutputSurface(QObject *parent, CoffeeFrameBufferObject *display);

    void setFramebuffer(CoffeeFrameBufferObject *display);
    void setFramebuffer(QObject* display);

    bool isBaked() const;

    QObject* framebufferQObject();
    CoffeeFrameBufferObject* getFramebuffer();


    QObject* getShader();

public slots:
    void addUiTexture(CoffeeTexture* tex);

    void setUniform(const QString &uniformName,ShaderVariant* data);
    void setFramebufferMapping(const QString &uniformName, int textureIndex, int textureUnit);

    void setWorld(CoffeeWorldOpts* value);
    void setMesh(CoffeeMesh* mesh);
    void bind();
    void load();
    void render();
    void unload();

    void setShader(QObject* shader);

protected:
    void setBaked(bool val);
    bool baked = false;

    CoffeeTexture* textureTest = nullptr;
    QVector<ShaderMapping*> uniforms;
    CoffeeFrameBufferObject* framebuffer;
    ShaderContainer* shader = nullptr;
    QVector<CoffeeOutputChannel*> textures;
    QPointer<CoffeeMesh> surface;
    QPointer<CoffeeWorldOpts> world;
};

#endif // COFFEEOUTPUTSURFACE_H
