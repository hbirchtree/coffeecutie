#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "engine/physics/physicsobject.h"
class CoffeeObject : public QObject
{
public:
    CoffeeObject(QObject *parent);

    virtual NumberContainer<glm::vec3>* position();
    virtual NumberContainer<glm::quat>* rotation();
    virtual NumberContainer<glm::vec3>* scale();
    virtual PhysicsObject* physics();

    virtual void render() = 0;
    virtual void unload() = 0;
    virtual void load() = 0;

protected:
    QPointer<PhysicsObject> physical;
    QPointer<NumberContainer<glm::vec3>> vposition;
    QPointer<NumberContainer<glm::quat>> qrotation;
    QPointer<NumberContainer<glm::vec3>> vscale;
};

#endif // COFFEEOBJECT_H
