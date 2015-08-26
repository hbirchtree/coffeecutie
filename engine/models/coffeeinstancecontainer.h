#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include <QMutex>

struct CoffeeInstanceChunk;
class PhysicsObject;

class CoffeeInstanceData : public QObject{
    Q_PROPERTY(QObject* position READ positionRef)
    Q_PROPERTY(QObject* rotation READ rotationRef)
    Q_PROPERTY(QObject* scale READ scaleRef)

    Q_OBJECT
public:
    CoffeeInstanceData(QObject* parent,CoffeeInstanceChunk* chunk);

    QObject* rotationRef();
    QObject* positionRef();
    QObject* scaleRef();

public slots:
    void bindObject(PhysicsObject* target);

private:
    Vector3Value *pos;
    QuatValue *rot;
    Vector3Value *scale;
};

struct CoffeeInstanceChunk
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)
//    Q_PROPERTY(QObjectList instanceObjects READ instanceObjects)

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
    uintptr_t getDataSize() const;

//    QObjectList instanceObjects() const;

    bool renderPrepare() const;

public slots:
    void createInstance();
    void addInstance(CoffeeInstanceChunk *i);
    void clearInstances();

    void setRenderPrepare(bool renderPrepare);

private:
    CoffeeInstanceChunk *createInstanceData();
    QVector<uintptr_t> m_touched;

    QObject *instanceAnchor;

    QVector<CoffeeInstanceChunk*> instances;
    bool m_renderPrepare = false;

    void* m_tmpBuffer = nullptr;
    uintptr_t m_tmpSize = 0;
};


#endif // COFFEEINSTANCECONTAINER_H
