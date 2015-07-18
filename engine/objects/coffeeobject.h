#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/physics/physicsobject.h"

class CoffeeObject : public QObject
{
    Q_OBJECT
public:
    /*
     * This is the base for a physics-enabled object
     * To support this, it has a position, rotation and scale that
     *  may be linked to the PhysicsObject
     *
     */

    virtual ~CoffeeObject();

    virtual Vector3Value* position();
    virtual QuatValue* rotation();
    virtual Vector3Value* scale();

    virtual PhysicsObject* physics();
    virtual void setPhysicsObject(PhysicsObject* obj);

public slots:
    virtual void render() = 0;
    virtual void unload() = 0;
    virtual void load() = 0;

protected:
    CoffeeObject(QObject* parent);
    QPointer<PhysicsObject> physical;
    QPointer<Vector3Value> vposition;
    QPointer<QuatValue> qrotation;
    QPointer<Vector3Value> vscale;
};

#endif // COFFEEOBJECT_H
