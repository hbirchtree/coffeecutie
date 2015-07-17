#include "vector4value.h"

Vector4Value::Vector4Value(QObject *parent, const glm::vec4 &initial) :
    VectorData(parent),
    NumberContainer<glm::vec4>(initial)
{
}

Vector4Value::Vector4Value(QObject *parent, std::function<glm::vec4 (const glm::vec4 &)> fun) :
    VectorData(parent),
    NumberContainer<glm::vec4>(fun)
{

}

void Vector4Value::setValue(const glm::vec4 &val)
{
    NumberContainer<glm::vec4>::setValue(val);
    valueChanged();
}

QVariantList Vector4Value::value() const
{
    glm::vec4 v = getValue();
    return QVariantList() << v.x << v.y << v.z << v.w;
}

void Vector4Value::unbindValue()
{
    disconnect(boundConnection);
    NumberContainer<glm::vec4>::unbindValue();
}

void Vector4Value::updateVectorData()
{
    if(!_tmp_vec_storage)
        _tmp_vec_storage = malloc(getVectorDataSize());
    glm::vec4 vec = getValue();
    memcpy(_tmp_vec_storage,&vec,getVectorDataSize());
}

const void *Vector4Value::getVectorData()
{
    updateVectorData();
    return _tmp_vec_storage;
}

uint32_t Vector4Value::getVectorDataSize() const
{
    return sizeof(glm::vec4);
}

QColor Vector4Value::asColor()
{
    glm::vec4 color = this->getValue();
    return QColor(qRgba(color.x*255,color.y*255,color.z*255,color.w*255));
}

void Vector4Value::fromColor(const QColor &color)
{
    glm::vec4 v;
    v.x = (float)color.red()/255.f;
    v.y = (float)color.green()/255.f;
    v.z = (float)color.blue()/255.f;
    v.w = (float)color.alpha()/255.f;
    setValue(v);
}

void Vector4Value::setValue(const QVariantList &value)
{
    if(value.size()!=4){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::vec4(value.at(0).toFloat(),value.at(1).toFloat(),value.at(2).toFloat(),value.at(3).toFloat()));
    valueChanged();
}

void Vector4Value::bindValue(Vector4Value *vec)
{
    boundConnection = connect(vec,&Vector4Value::valueChanged,[=](){
        valueChanged();
    });
    NumberContainer<glm::vec4>::bindValue(((NumberContainer<glm::vec4>*)vec));
}
