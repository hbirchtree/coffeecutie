#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include <QMutex>

class PhysicsObject;

class CoffeeInstanceData : public QObject{
    Q_PROPERTY(QObject* position READ positionRef)
    Q_PROPERTY(QObject* rotation READ rotationRef)
    Q_PROPERTY(QObject* scale READ scaleRef)

    Q_PROPERTY(QObject* physics READ physicsQObject)

    Q_OBJECT
public:
    CoffeeInstanceData(glm::vec3 pos,glm::quat rot, glm::vec3 scale, QObject* parent);

    Vector3Value* getPos();
    Vector3Value* getScale();
    QuatValue *getRot();
    PhysicsObject* physics();

    QObject* physicsQObject();

    QObject* rotationRef();
    QObject* positionRef();
    QObject* scaleRef();

public slots:
    void bindObject(PhysicsObject* target);

private:
    QPointer<PhysicsObject> m_physics;
    Vector3Value *pos;
    QuatValue *rot;
    Vector3Value *scale;
};

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)
    Q_PROPERTY(QObjectList instanceObjects READ instanceObjects)

    //used to avoid new instances being created before rendering.
    //if there are, it will fail catastrophically because the object does not have any data in its model-matrix buffer
    Q_PROPERTY(bool renderPrepare READ renderPrepare WRITE setRenderPrepare)

    Q_OBJECT
public:
    CoffeeInstanceContainer(QObject *parent);
    ~CoffeeInstanceContainer();

    uint instanceCount() const;

    Q_INVOKABLE CoffeeInstanceData* getInstance(int index);
    Q_INVOKABLE QObject* getInstanceQObject(int index);
    QVector<glm::mat4> getData() const;

    QObjectList instanceObjects() const;

    bool renderPrepare() const;

public slots:
    void createInstance();
    void addInstance(CoffeeInstanceData* i);
    void clearInstances();

    void setRenderPrepare(bool renderPrepare);

private:
    CoffeeInstanceData* createInstanceData();

    QObject *instanceAnchor;

    QVector<CoffeeInstanceData*> instances;
    bool m_renderPrepare = false;
};


#endif // COFFEEINSTANCECONTAINER_H
