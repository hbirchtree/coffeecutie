#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/objects/coffeeobject.h"
#include "opengl/components/coffeematerial.h" //These are needed for the property system
#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeemesh.h"
#include "engine/shaders/coffeeuniformsetter.h"

class ShaderVariant;

class CoffeeStandardObject : public QObject,public CoffeeObject, public CoffeeUniformSetter
{
    Q_PROPERTY(QObject* position READ positionValue)
    Q_PROPERTY(QObject* rotation READ rotationValue)
    Q_PROPERTY(QObject* scale READ scaleValue)

    Q_PROPERTY(QObject* material READ materialRef)
    Q_PROPERTY(QObject* mesh READ meshRef)
    Q_PROPERTY(QObject* shader READ shaderRef WRITE setShaderRef)

    Q_PROPERTY(QObject* physics READ physicsRef WRITE setPhysicsObject)
    Q_PROPERTY(bool hasPhysics READ hasPhysics)

    Q_PROPERTY(QObject* matrix READ getModelMatrix)

    Q_OBJECT

public:
    CoffeeStandardObject(QObject* parent);
    ~CoffeeStandardObject();

    Q_INVOKABLE CoffeeMesh* mesh();
    void setMesh(CoffeeMesh* mesh);
    Q_INVOKABLE CoffeeShader* shader();
    void setShader(CoffeeShader* shader);
    Q_INVOKABLE CoffeeMaterial* material();
    void setMaterial(CoffeeMaterial *mtl);

    QObject* materialRef();
    QObject* meshRef();
    QObject* shaderRef();

    QObject* positionValue();
    QObject* rotationValue();
    QObject* scaleValue();

    QObject* physicsRef();
    QObject* getModelMatrix() const;

    bool hasPhysics();

public slots:
    void render();
    void unload();
    void load();

    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);

    void setPhysicsObject(QObject* obj);
    void setShaderRef(QObject* sh);

    //We want these to work as slots, slots inherited from non-QObject classes do not work.
    void setUniform(QString uniformName, ShaderVariant* data);
    void setTexture(QString samplerName, CoffeeTexture *texture);

protected:
    ShaderVariant* modelMatrix;
    bool baked = false;
    QPointer<CoffeeMesh> pmesh;
    QPointer<CoffeeMaterial> pmaterial;
};

#endif // COFFEESTANDARDOBJECT_H
