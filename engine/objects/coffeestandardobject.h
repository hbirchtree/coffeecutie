#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/objects/coffeeobject.h"
#include "opengl/components/coffeematerial.h" //These are needed for the property system
#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeemesh.h"
#include "engine/shaders/coffeeuniformsetter.h"

class CoffeeStandardObject : public CoffeeObject
{
    Q_PROPERTY(QObject* material READ materialRef WRITE setMaterialRef)
    Q_PROPERTY(QObject* mesh READ meshRef WRITE setMeshRef)

    Q_PROPERTY(QObject* matrix READ getModelMatrix)

    Q_OBJECT

public:
    CoffeeStandardObject(QObject* parent);
    ~CoffeeStandardObject();

    Q_INVOKABLE CoffeeMesh* mesh();
    void setMesh(CoffeeMesh* mesh);
    Q_INVOKABLE CoffeeMaterial* material();
    void setMaterial(CoffeeMaterial *mtl);

    QObject* materialRef();
    QObject* meshRef();

    QObject* getModelMatrix() const;

public slots:
    void render();
    void unload();
    void load();

    void setMeshRef(QObject* me);
    void setMaterialRef(QObject* ma);

protected:
    Matrix4Value* modelMatrix;
    QPointer<CoffeeMesh> m_mesh;
    QPointer<CoffeeMaterial> m_material;
};

#endif // COFFEESTANDARDOBJECT_H
