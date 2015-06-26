#include "qscriptvectorvalue.h"

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

void Vector3Value::bindValue(Vector3Value *vec)
{
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
}

void Vector3Value::setValue(const QVariantList &value)
{
    if(value.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::vec3(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat()));
}

void Vector3Value::setAcceleration(const QVariantList &value)
{
    if(value.size()!=3){
        qWarning("Invalid argument!");
        return;
    }
    setAcceleration(glm::vec3(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat()));
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

QVariantList QuatValue::value() const
{
    glm::quat q = getValue();
    return QVariantList() << q.w << q.x << q.y << q.z;
}

void QuatValue::mathCumulate(const QVariantList &vals)
{
    if(vals.size()!=4){
        qWarning("Invalid argument!");
        return;
    }
    NumberContainer<glm::quat>::operator=(glm::quat(vals.at(0).toFloat(),
                                                    vals.at(1).toFloat(),
                                                    vals.at(2).toFloat(),
                                                    vals.at(3).toFloat())*getValue());
}

void QuatValue::mathCumulate(QuatValue *val)
{
    NumberContainer<glm::quat>::operator=(val->getValue()*getValue());
}

void QuatValue::mathNormalize()
{
    NumberContainer<glm::quat>::operator=(glm::normalize(getValue()));
}

void QuatValue::setValue(const QVariantList &value)
{
    if(value.size()!=4){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::quat(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat(),value.at(3).toFloat()));
}


ScalarValue::ScalarValue(QObject *parent, ScalarDataType v) :
    QObject(parent),
    NumberContainer<ScalarDataType>(v)
{
}

ScalarDataType ScalarValue::value() const
{
    return getValue();
}

void ScalarValue::setValue(ScalarDataType value)
{
    NumberContainer<ScalarDataType>::setValue(value);
}

void ScalarValue::bindValue(ScalarValue *val)
{
    NumberContainer<ScalarDataType>::bindValue(((NumberContainer<ScalarDataType>*)val));
}
