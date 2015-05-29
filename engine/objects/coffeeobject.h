#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include <QtPlugin>
#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "engine/physics/physicsobject.h"
class CoffeeObject : public QObject
{
public:
    /*
     * This is the base for a physics-enabled object
     * To support this, it has a position, rotation and scale that
     *  may be linked to the PhysicsObject
     *
     */
    CoffeeObject(QObject *parent);

    virtual ~CoffeeObject();

    virtual NumberContainer<glm::vec3>* position();
    virtual NumberContainer<glm::quat>* rotation();
    virtual NumberContainer<glm::vec3>* scale();

    virtual PhysicsObject* physics();
    virtual void setPhysicsObject(PhysicsObject* obj);

public slots:
    virtual void render() = 0;
    virtual void unload() = 0;
    virtual void load() = 0;

protected:
    QPointer<PhysicsObject> physical;
    QPointer<NumberContainer<glm::vec3>> vposition;
    QPointer<NumberContainer<glm::quat>> qrotation;
    QPointer<NumberContainer<glm::vec3>> vscale;
};

#define CoffeeObjectIID "com.hbirchtree.coffeecutie.CoffeeObject/2.0"

Q_DECLARE_INTERFACE(CoffeeObject,CoffeeObjectIID)

#endif // COFFEEOBJECT_H
