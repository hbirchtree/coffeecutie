#ifndef COFFEEINSTANCEDOBJECT_H
#define COFFEEINSTANCEDOBJECT_H

#include "engine/objects/coffeeobject.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/numberbuffer.h"
class CoffeeInstancedObject : public CoffeeObject
{
    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)
public:
    CoffeeInstancedObject(QObject *parent);

public slots:
    void render();
    void unload();
    void load();
};

#endif // COFFEEINSTANCEDOBJECT_H
