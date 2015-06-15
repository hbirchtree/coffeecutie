#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"
#include "general/data/numbercontainer.h"

class VectorValue;
class QuaternionValue;
class PhysicsObject;

class CoffeeInstanceData : public QObject{
    Q_PROPERTY(QObject* position READ positionRef)
    Q_PROPERTY(QObject* rotation READ rotationRef)
    Q_PROPERTY(QObject* scale READ scaleRef)

    Q_PROPERTY(QObject* physics READ physicsQObject)

    Q_OBJECT
public:
    CoffeeInstanceData(glm::vec3 pos,glm::quat rot, glm::vec3 scale, QObject* parent);

    NumberContainer<glm::vec3>* getPos();
    NumberContainer<glm::vec3>* getScale();
    NumberContainer<glm::quat>* getRot();
    PhysicsObject* physics();

    QObject* physicsQObject();

    QObject* rotationRef();
    QObject* positionRef();
    QObject* scaleRef();

public slots:
    void bindObject(PhysicsObject* target);

private:
    QPointer<PhysicsObject> m_physics;
    NumberContainer<glm::vec3> *pos;
    NumberContainer<glm::quat> *rot;
    NumberContainer<glm::vec3> *scale;

    VectorValue* posWrapper;
    QuaternionValue* rotWrapper;
    VectorValue* sclWrapper;
};

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)
    Q_PROPERTY(QObjectList instanceObjects READ instanceObjects)

    Q_OBJECT
public:


    CoffeeInstanceContainer(QObject *parent);
    ~CoffeeInstanceContainer();

    uint instanceCount() const;
    Q_INVOKABLE CoffeeInstanceData* getInstance(int index);
    Q_INVOKABLE QObject* getInstanceQObject(int index);
    QVector<glm::mat4> getData() const;

    QObjectList instanceObjects() const;

public slots:
    void createInstance();
    void addInstance(CoffeeInstanceData* i);
    void clearInstances();

private:
    CoffeeInstanceData* createInstanceData();

    QObject *instanceAnchor;

    QVector<CoffeeInstanceData*> instances;
};


#endif // COFFEEINSTANCECONTAINER_H
