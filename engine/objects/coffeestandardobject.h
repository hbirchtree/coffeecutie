#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/objects/coffeeobject.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/shadercontainer.h"
class CoffeeStandardObject : public CoffeeObject
{
public:
    CoffeeStandardObject(QObject* parent);

    void render();
    void unload();
    void load();

    CoffeeMesh* mesh();
    ShaderContainer* shader();
    CoffeeMaterial* material();

protected:
    QPointer<CoffeeMesh> pmesh;
    QPointer<ShaderContainer> pshader;
    QPointer<CoffeeMaterial> pmaterial;
};

#endif // COFFEESTANDARDOBJECT_H
