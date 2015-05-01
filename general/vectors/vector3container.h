#ifndef VECTOR3CONTAINER_H
#define VECTOR3CONTAINER_H

#include "general/common.h"

class Vector3Container : public QObject
{
    Q_OBJECT
    //We create a meta-class for the purpose of more dynamic behavior
public:
    explicit Vector3Container(QObject *parent = 0);
    Vector3Container(QPointer<Vector3Container> bound);

    ~Vector3Container();

    glm::vec3 getValue();
    void setValue(const glm::vec3 &value);

    glm::vec3 getVelocity();
    void setVelocity(const glm::vec3 &value);

    glm::vec3 getAcceleration();
    void setAcceleration(const glm::vec3 &value);

    void setClamps(glm::vec3 min,glm::vec3 max);
    glm::vec3 getMinClamp();
    glm::vec3 getMaxClamp();

    glm::vec3 animationVelocity;
    glm::vec3 animationAcceleration;

    std::function<glm::vec3()> getValueOffset();
    void setValueOffset(std::function<glm::vec3()> func);

    void bindValue(QPointer<Vector3Container> value);

    static QString vec3toString(glm::vec3 v){
        return QString("%1,%2,%3")
                .arg(v.x)
                .arg(v.y)
                .arg(v.z);
    }

signals:
    void valueChanged(glm::vec3 newValue);
    void velocityChanged(glm::vec3 newVelo);
    void accelerationChanged(glm::vec3 newAccel);

private:
    void unbindValue(){
        bound = 0;
    }

    std::function<glm::vec3()> valueOffsetCallback = [](){
        return glm::vec3(0,0,0);
    };
    QPointer<Vector3Container> bound;
    glm::vec3 value;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    //We store these here for convenience. We will not modify the value.
    glm::vec3 valmin;
    glm::vec3 valmax;
};

#endif // VECTOR3CONTAINER_H
