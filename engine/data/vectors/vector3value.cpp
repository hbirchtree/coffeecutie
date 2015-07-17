#include "vector3value.h"

Vector3Value::Vector3Value(QObject *parent, const glm::vec3 &initial) :
    VectorData(parent),
    NumberContainer<glm::vec3>(initial)
{
}

Vector3Value::Vector3Value(QObject *parent, std::function<glm::vec3 (const glm::vec3 &)> fun):
    VectorData(parent),
    NumberContainer<glm::vec3>(fun)
{
}

Vector3Value::Vector3Value(QObject *parent, float x, float y, float z) :
    VectorData(parent),
    NumberContainer<glm::vec3>(glm::vec3(x,y,z))
{
}

void Vector3Value::bindValue(Vector3Value *vec)
{
    boundConnection = connect(vec,&Vector3Value::valueChanged,[=](){
        valueChanged();
    });
    NumberContainer<glm::vec3>::bindValue(((NumberContainer<glm::vec3>*)vec));
}

QVariantList Vector3Value::velocity() const
{
    glm::vec3 q = getVelocity();
    return QVariantList() << q.x << q.y << q.z;
}

QVariantList Vector3Value::value() const
{
    glm::vec3 q = getValue();
    return QVariantList() << q.x << q.y << q.z;
}

QVariantList Vector3Value::acceleration() const
{
    glm::vec3 q = getAcceleration();
    return QVariantList() << q.x << q.y << q.z;
}

void Vector3Value::setValue(const glm::vec3 &val)
{
    NumberContainer<glm::vec3>::setValue(val);
    valueChanged();
}

void Vector3Value::unbindValue()
{
    disconnect(boundConnection);
    NumberContainer<glm::vec3>::unbindValue();
}

void Vector3Value::updateVectorData()
{
    if(!_tmp_vec_storage)
        _tmp_vec_storage = malloc(getVectorDataSize());
    glm::vec3 vec = getValue();
    memcpy(_tmp_vec_storage,&vec,getVectorDataSize());
}

const void *Vector3Value::getVectorData()
{
    updateVectorData();
    return _tmp_vec_storage;
}

uint32_t Vector3Value::getVectorDataSize() const
{
    return sizeof(glm::vec3);
}

void Vector3Value::mathMultiply(const QVariantList &vals)
{
    if(vals.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    glm::vec3 v2(vals.at(0).toFloat(),vals.at(1).toFloat(),vals.at(2).toFloat());
    setValue(getValue()*v2);
}

void Vector3Value::mathMultiply(Vector3Value *val)
{
    NumberContainer<glm::vec3>::operator*=(val->getValue());
}

void Vector3Value::mathAdd(const QVariantList &vals)
{
    if(vals.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    glm::vec3 v2(vals.at(0).toFloat(),vals.at(1).toFloat(),vals.at(2).toFloat());
    setValue(getValue()+v2);
}

void Vector3Value::mathAdd(Vector3Value *val)
{
    NumberContainer<glm::vec3>::operator+=(val->getValue());
}

void Vector3Value::mathNormalize()
{
    setValue(glm::normalize(getValue()));
}

void Vector3Value::setVelocity(const QVariantList &velocity)
{
    if(velocity.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    setVelocity(glm::vec3(velocity.at(0).toFloat(),velocity.at(1).toFloat(),velocity.at(2).toFloat()));
    velocityChanged();
}

void Vector3Value::setValue(const QVariantList &value)
{
    if(value.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::vec3(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat()));
    valueChanged();
}

void Vector3Value::setAcceleration(const QVariantList &value)
{
    if(value.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    setAcceleration(glm::vec3(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat()));
    accelerationChanged();
}
