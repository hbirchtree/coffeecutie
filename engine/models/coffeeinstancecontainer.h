#ifndef COFFEEINSTANCECONTAINER_H
#define COFFEEINSTANCECONTAINER_H

#include <QObject>
#include "general/common.h"

class CoffeeInstanceContainer : public QObject
{
    Q_PROPERTY(uint instanceCount READ instanceCount)

    Q_OBJECT
public:

    class InstanceData{
    public:
        InstanceData(glm::vec3 pos,glm::quat rot, glm::vec3 scale){
            this->pos = pos;
            this->rot = rot;
            this->scale = scale;
        }

        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 scale;
    };

    CoffeeInstanceContainer(QObject *parent);

    uint instanceCount() const;

    InstanceData* getInstance(uint index);

    QVector<glm::mat4> getData() const;

private:
    QVector<InstanceData*> instances;
};

#endif // COFFEEINSTANCECONTAINER_H
