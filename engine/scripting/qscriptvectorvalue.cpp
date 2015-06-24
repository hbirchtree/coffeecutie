#include "qscriptvectorvalue.h"

VectorValue::VectorValue(QObject* parent, NumberContainer<glm::vec3> *source) : QObject(parent)
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

QVariantList VectorValue::position() const
{
    glm::vec3 p = source->getValue();
    return QVariantList() << p.x << p.y << p.z;
}

QVariantList VectorValue::velocity() const
{
    glm::vec3 p = source->getVelocity();
    return QVariantList() << p.x << p.y << p.z;
}

QVariantList VectorValue::acceleration() const
{
    glm::vec3 p = source->getAcceleration();
    return QVariantList() << p.x << p.y << p.z;
}

NumberContainer<glm::vec3> *VectorValue::getRaw()
{
    return source;
}

void VectorValue::bind(VectorValue *v)
{
    this->source->bindValue(v->getRaw());
}

void VectorValue::unbind()
{
    source->unbindValue();
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

void VectorValue::setPositionList(QVariantList position)
{
    if(position.size()!=3)
        return;
    source->setValue(glm::vec3(position.at(0).toFloat(),position.at(1).toFloat(),position.at(2).toFloat()));
}

void VectorValue::setVelocityList(QVariantList velocity)
{
    if(velocity.size()!=3)
        return;
    source->setVelocity(glm::vec3(velocity.at(0).toFloat(),velocity.at(1).toFloat(),velocity.at(2).toFloat()));
}

void VectorValue::setAccelerationList(QVariantList acceleration)
{
    if(acceleration.size()!=3)
        return;
    source->setAcceleration(glm::vec3(acceleration.at(0).toFloat(),acceleration.at(1).toFloat(),acceleration.at(2).toFloat()));
}

void VectorValue::pMult(float c)
{
    *source*=c;
}

void VectorValue::pMult(float x, float y, float z)
{
    *source*=glm::vec3(x,y,z);
}

void VectorValue::pAdd(float x, float y, float z)
{
    *source+=glm::vec3(x,y,z);
}

QuaternionValue::QuaternionValue(QObject *parent, NumberContainer<glm::quat> *source) : QObject(parent)
{
    this->source = source;
}

QVariantList QuaternionValue::value() const
{
    glm::quat p = source->getValue();
    return QVariantList() << p.w << p.x << p.y << p.z;
}

NumberContainer<glm::quat> *QuaternionValue::getRaw()
{
    return source;
}

void QuaternionValue::bind(QuaternionValue *v)
{
    this->source->bindValue(v->getRaw());
}

void QuaternionValue::unbind()
{
    source->unbindValue();
}

void QuaternionValue::setValue(QVariantList value)
{
    if(value.size()!=4)
        return;
    source->setValue(glm::quat(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat(),value.at(3).toFloat()));
}


Vector3Value::Vector3Value(QObject *parent, const glm::vec3 &initial) :
    QObject(parent),
    NumberContainer<glm::vec3>(initial)
{
}

Vector3Value::Vector3Value(QObject *parent, float x, float y, float z) :
    QObject(parent),
    NumberContainer<glm::vec3>(glm::vec3(x,y,z))
{
}

void Vector3Value::bindValue(QPointer<Vector3Value> vec)
{
    NumberContainer<glm::vec3>::bindValue(((NumberContainer<glm::vec3>*)vec));
}

QuatValue::QuatValue(QObject *parent, const glm::quat &initial) :
    QObject(parent),
    NumberContainer<glm::quat>(initial)
{
}

QuatValue::QuatValue(QObject *parent, float w, float x, float y, float z) :
    QObject(parent),
     NumberContainer<glm::quat>(glm::quat(w,x,y,z))
{
}

void QuatValue::bindValue(QPointer<QuatValue> quat)
{
    NumberContainer<glm::quat>::bindValue(((NumberContainer<glm::quat>*)quat));
}


ScalarValue::ScalarValue(QObject *parent, float v) :
    QObject(parent),
    NumberContainer<float>(v)
{
}
