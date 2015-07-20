#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/physics/physicsobject.h"
#include "engine/shaders/coffeeuniformsetter.h"

class CoffeeObject : public QObject,public CoffeeUniformSetter
{
    Q_PROPERTY(QObject* position READ positionValue)
    Q_PROPERTY(QObject* rotation READ rotationValue)
    Q_PROPERTY(QObject* scale READ scaleValue)

    Q_PROPERTY(QObject* shader READ shaderRef WRITE setShaderRef)

    Q_PROPERTY(bool hasPhysics READ hasPhysics)
    Q_PROPERTY(QObject* physics READ physicsRef WRITE setPhysicsObject)

    Q_PROPERTY(bool baked READ baked)

    Q_OBJECT

private:
    bool m_baked;

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

    bool baked() const;
    bool hasPhysics() const;

    QObject* positionValue();
    QObject* rotationValue();
    QObject* scaleValue();

    QObject* physicsRef();
    QObject* shaderRef();

    Q_INVOKABLE QString toString();

public slots:
    virtual void render() = 0;
    virtual void unload() = 0;
    virtual void load() = 0;

    //We want these to work as slots, slots inherited from non-QObject classes do not work.
    void setUniform(QString uniformName, QObject *data);
    void setTexture(QString samplerName, CoffeeTexture *texture);

    void setPhysicsObject(QObject* obj);
    void setShaderRef(QObject* sh);

protected:
    void setBaked(bool baked);

    CoffeeObject(QObject* parent);
    QPointer<PhysicsObject> m_physics;
    QPointer<Vector3Value> m_position;
    QPointer<QuatValue> m_rotation;
    QPointer<Vector3Value> m_scale;
};

#endif // COFFEEOBJECT_H
