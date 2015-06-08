#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"
#include "general/data/numbercontainer.h"

class PhysicsObject;

class CoffeeInstanceData : public QObject{

    Q_OBJECT
public:
    CoffeeInstanceData(glm::vec3 pos,glm::quat rot, glm::vec3 scale, QObject* parent) : QObject(parent){
        this->pos = new NumberContainer<glm::vec3>(this,pos);
        this->scale = new NumberContainer<glm::vec3>(this,scale);
        this->rot = new NumberContainer<glm::quat>(this,rot);
    }

    NumberContainer<glm::vec3>* getPos(){ return pos;}
    NumberContainer<glm::vec3>* getScale(){ return scale;}
    NumberContainer<glm::quat>* getRot(){ return rot;}
    PhysicsObject* physics();

public slots:
    void bindObject(PhysicsObject* target);

private:
    QPointer<PhysicsObject> m_physics;
    NumberContainer<glm::vec3> *pos;
    NumberContainer<glm::quat> *rot;
    NumberContainer<glm::vec3> *scale;
};

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)

    Q_OBJECT
public:


    CoffeeInstanceContainer(QObject *parent);
    ~CoffeeInstanceContainer();

    uint instanceCount() const;
    CoffeeInstanceData* getInstance(int index);
    QVector<glm::mat4> getData() const;

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
