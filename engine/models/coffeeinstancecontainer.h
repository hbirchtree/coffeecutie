#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"
#include "general/data/numbercontainer.h"

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)

    Q_OBJECT
public:

    class InstanceData{
    public:
        InstanceData(glm::vec3 pos,glm::quat rot, glm::vec3 scale, QObject* parent){
            this->pos = new NumberContainer<glm::vec3>(parent,pos);
            this->scale = new NumberContainer<glm::vec3>(parent,scale);
            this->rot = new NumberContainer<glm::quat>(parent,rot);
        }

        NumberContainer<glm::vec3>* getPos(){ return pos;}
        NumberContainer<glm::vec3>* getScale(){ return scale;}
        NumberContainer<glm::quat>* getRot(){ return rot;}

    private:
        NumberContainer<glm::vec3> *pos;
        NumberContainer<glm::quat> *rot;
        NumberContainer<glm::vec3> *scale;
    };

    CoffeeInstanceContainer(QObject *parent);
    ~CoffeeInstanceContainer();

    uint instanceCount() const;
    InstanceData* getInstance(uint index);
    QVector<glm::mat4> getData() const;

private:
    QObject *instanceAnchor;

    QVector<InstanceData*> instances;
};

#endif // COFFEEINSTANCECONTAINER_H
