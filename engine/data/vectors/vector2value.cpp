#include "vector2value.h"

Vector2Value::Vector2Value(QObject *parent, const glm::vec2 &initial) :
    VectorData(parent),
    NumberContainer<glm::vec2>(initial)
{
}

Vector2Value::Vector2Value(QObject *parent, std::function<glm::vec2 (const glm::vec2 &)> fun) :
    VectorData(parent),
    NumberContainer<glm::vec2>(fun)
{

}

Vector2Value::Vector2Value(QObject *parent, float x, float y) :
    Vector2Value(parent,glm::vec2(x,y))
{
}

void Vector2Value::setValue(const glm::vec2 &val)
{
    NumberContainer<glm::vec2>::setValue(val);
    valueChanged();
}

QVariantList Vector2Value::value() const
{
    glm::vec2 v = getValue();
    return QVariantList() << v.x << v.y;
}

void Vector2Value::unbindValue()
{
    disconnect(boundConnection);
    NumberContainer<glm::vec2>::unbindValue();
}

void Vector2Value::updateVectorData()
{
    if(!_tmp_vec_storage)
        _tmp_vec_storage = malloc(getVectorDataSize());
    glm::vec2 vec = getValue();
    memcpy(_tmp_vec_storage,&vec,getVectorDataSize());
}

const void *Vector2Value::getVectorData()
{
    updateVectorData();
    return _tmp_vec_storage;
}

uint32_t Vector2Value::getVectorDataSize() const
{
    return sizeof(glm::vec2);
}

void Vector2Value::setValue(const QVariantList &value)
{
    if(value.size()!=2){
        qWarning("Invalid argument!");
        return;
    }
    setValue(glm::vec2(value.at(0).toFloat(),value.at(1).toFloat()));
    valueChanged();
}

void Vector2Value::bindValue(Vector2Value *vec)
{
    boundConnection = connect(vec,&Vector2Value::valueChanged,[=](){
        valueChanged();
    });
    NumberContainer<glm::vec2>::bindValue(((NumberContainer<glm::vec2>*)vec));
}
