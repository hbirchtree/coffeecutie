#ifndef VECTOR3CONTAINER_H
#define VECTOR3CONTAINER_H

#include <QObject>
#include <QPointer>
#include <glm/vec3.hpp>

class Vector3Container : public QObject
{
    Q_OBJECT
    //We create a meta-class for the purpose of more dynamic behavior
public:
    explicit Vector3Container(QObject *parent = 0);
    Vector3Container(QPointer<Vector3Container> bound);

    ~Vector3Container();

    glm::vec3 getValue();
    void setValue(glm::vec3 value);

    glm::vec3 getVelocity();
    void setVelocity(const glm::vec3 &value);

    glm::vec3 getAcceleration();
    void setAcceleration(const glm::vec3 &value);

    glm::vec3 animationVelocity;
    glm::vec3 animationAcceleration;

    std::function<glm::vec3()> getValueOffset();
    void setValueOffset(std::function<glm::vec3()> func);

    void bindValue(QPointer<Vector3Container> value);
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

};

#endif // VECTOR3CONTAINER_H
