#include "qscriptvectorvalue.h"

VectorValue::VectorValue(NumberContainer<glm::vec3> *source) : QObject(source)
{
    this->source = source;
}

float VectorValue::z() const
{
    return source->getValue().z;
}

float VectorValue::y() const
{
    return source->getValue().y;
}

float VectorValue::x() const
{
    return source->getValue().x;
}

float VectorValue::velocityZ() const
{
    return source->getVelocity().z;
}

float VectorValue::velocityY() const
{
    return source->getVelocity().y;
}

float VectorValue::velocityX() const
{
    return source->getVelocity().x;
}

float VectorValue::accelerationX() const
{
    return source->getAcceleration().x;
}

float VectorValue::accelerationY() const
{
    return source->getAcceleration().y;
}

float VectorValue::accelerationZ() const
{
    return source->getAcceleration().z;
}

QList<float> VectorValue::position() const
{
    glm::vec3 p = source->getValue();
    return QList<float>() << p.x << p.y << p.z;
}

QList<float> VectorValue::velocity() const
{
    glm::vec3 p = source->getVelocity();
    return QList<float>() << p.x << p.y << p.z;
}

QList<float> VectorValue::acceleration() const
{
    glm::vec3 p = source->getAcceleration();
    return QList<float>() << p.x << p.y << p.z;
}

void VectorValue::setPosition(float x, float y, float z)
{
    source->setValue(glm::vec3(x,y,z));
}

void VectorValue::setVelocity(float x, float y, float z)
{
    source->setVelocity(glm::vec3(x,y,z));
}

void VectorValue::setAcceleration(float x, float y, float z)
{
    source->setAcceleration(glm::vec3(x,y,z));
}

void VectorValue::setZ(float z)
{
    glm::vec3 t = source->getValue();
    t.z = z;
    *source = t;
}

void VectorValue::setY(float y)
{
    glm::vec3 t = source->getValue();
    t.y = y;
    *source = t;
}

void VectorValue::setX(float x)
{
    glm::vec3 t = source->getValue();
    t.x = x;
    *source = t;
}

void VectorValue::setVelocityZ(float velocityZ)
{
    glm::vec3 t = source->getVelocity();
    t.z = velocityZ;
    source->setVelocity(t);
}

void VectorValue::setVelocityY(float velocityY)
{
    glm::vec3 t = source->getVelocity();
    t.y = velocityY;
    source->setVelocity(t);
}

void VectorValue::setVelocityX(float velocityX)
{
    glm::vec3 t = source->getVelocity();
    t.x = velocityX;
    source->setVelocity(t);
}

void VectorValue::setAccelerationX(float accelerationX)
{
    glm::vec3 t = source->getAcceleration();
    t.x = accelerationX;
    source->setAcceleration(t);
}

void VectorValue::setAccelerationY(float accelerationY)
{
    glm::vec3 t = source->getAcceleration();
    t.y = accelerationY;
    source->setAcceleration(t);
}

void VectorValue::setAccelerationZ(float accelerationZ)
{
    glm::vec3 t = source->getAcceleration();
    t.z = accelerationZ;
    source->setAcceleration(t);
}

void VectorValue::setPositionList(QList<float> position)
{
    if(position.size()!=3)
        return;
    source->setValue(glm::vec3(position.at(0),position.at(1),position.at(2)));
}

void VectorValue::setVelocityList(QList<float> velocity)
{
    if(velocity.size()!=3)
        return;
    source->setVelocity(glm::vec3(velocity.at(0),velocity.at(1),velocity.at(2)));
}

void VectorValue::setAccelerationList(QList<float> acceleration)
{
    if(acceleration.size()!=3)
        return;
    source->setAcceleration(glm::vec3(acceleration.at(0),acceleration.at(1),acceleration.at(2)));
}


QuaternionValue::QuaternionValue(NumberContainer<glm::quat> *source) : QObject(source)
{
    this->source = source;
}

QList<float> QuaternionValue::value() const
{
    glm::quat p = source->getValue();
    return QList<float>() << p.w << p.x << p.y << p.z;
}

void QuaternionValue::setValue(QList<float> value)
{
    if(value.size()!=4)
        return;
    source->setValue(glm::quat(value.at(0),value.at(1),value.at(2),value.at(3)));
}
