#ifndef COFFEEINSTANCEDOBJECT_H
#define COFFEEINSTANCEDOBJECT_H

#include <engine/objects/coffeesimpleobject.h>
class CoffeeInstancedObject : public CoffeeSimpleObject
{
public:
    CoffeeInstancedObject(QObject *parent);

    // CoffeeSimpleObject interface
public:
    void render();
};

#endif // COFFEEINSTANCEDOBJECT_H
