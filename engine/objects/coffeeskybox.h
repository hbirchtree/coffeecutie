#ifndef COFFEESKYBOX_H
#define COFFEESKYBOX_H

#include "coffeeobject.h"

class CoffeeMesh;
class CoffeeCamera;
class ShaderContainer;
class CoffeeTexture;

class CoffeeSkybox : public QObject,public CoffeeObject
{
    Q_OBJECT
public:
    CoffeeSkybox(QObject* parent = 0, CoffeeCamera *camera = 0);

    void addMap(GLenum side,QString source);
    void setCamera(CoffeeCamera* camera);

    QPointer<CoffeeTexture> getTexture() const;
    void setTexture(QPointer<CoffeeTexture> value);

    QPointer<CoffeeMesh> getSkymesh() const;
    void setSkymesh(QPointer<CoffeeMesh> value);

    QPointer<ShaderContainer> getShader() const;
    void setShader(QPointer<ShaderContainer> value);

public slots:
    void render();
    void unload();
    void load();

protected:
    bool baked = false;

    QMap<GLenum,QString> cubemapping;

    QPointer<CoffeeCamera> camera;
    QPointer<ShaderContainer> shader;
    QPointer<CoffeeTexture> texture;
    QPointer<CoffeeMesh> skymesh;

    GLuint buffs[2];
    GLuint vao;
};

#endif // COFFEESKYBOX_H
