#include "quatvalue.h"

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

void QuatValue::bindValue(QuatValue* quat)
{
    boundConnection = connect(quat,&QuatValue::valueChanged,[=](){
        valueChanged();
    });
    NumberContainer<glm::quat>::bindValue(((NumberContainer<glm::quat>*)quat));
}

QVariantList QuatValue::value() const
{
    glm::quat q = getValue();
    return QVariantList() << q.w << q.x << q.y << q.z;
}

void QuatValue::setValue(const glm::quat &val)
{
    NumberContainer<glm::quat>::setValue(val);
    valueChanged();
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
    valueChanged();
}

void QuatValue::mathCumulate(QuatValue *val)
{
    NumberContainer<glm::quat>::operator=(val->getValue()*getValue());
    valueChanged();
}

void QuatValue::mathNormalize()
{
    NumberContainer<glm::quat>::operator=(glm::normalize(getValue()));
    valueChanged();
}

void QuatValue::setValue(const QVariantList &value)
{
    if(value.size()!=4){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::quat(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat(),value.at(3).toFloat()));
    valueChanged();
}
