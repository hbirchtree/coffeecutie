#include "matrix4value.h"

Matrix4Value::Matrix4Value(QObject *parent) :
    VectorData(parent),
    NumberContainer<glm::mat4>(glm::mat4())
{
}

Matrix4Value::Matrix4Value(QObject *parent, std::function<glm::mat4 (const glm::mat4 &)> fun) :
    VectorData(parent),
    NumberContainer<glm::mat4>(fun)
{
}

float Matrix4Value::getValue(uint col, uint row)
{
    return NumberContainer<glm::mat4>::getValue()[col][row];
}

void Matrix4Value::unbindValue()
{
    VectorData::bindSignals(nullptr);
    NumberContainer<glm::mat4>::unbindValue();
}

void Matrix4Value::updateVectorData()
{
    if(!_tmp_vec_storage)
        _tmp_vec_storage = malloc(getVectorDataSize());
    glm::mat4 vec = getValue();
    memcpy(_tmp_vec_storage,&vec,getVectorDataSize());
}

uint32_t Matrix4Value::getVectorDataSize() const
{
    return sizeof(glm::mat4);
}

QVariantMap Matrix4Value::values() const
{
    glm::mat4 val = getValue();
    QVariantMap map;

    for(int i=0;i<4;i++){
        QVariantMap child;
        for(int j=0;j<4;j++)
            child.insert(QString::number(j),val[i][j]);
        map.insert(QString::number(i),child);
    }

    return map;
}

void Matrix4Value::setValue(const glm::mat4 &val)
{
    NumberContainer<glm::mat4>::setValue(val);
    valueChanged();
}

void Matrix4Value::setValue(Matrix4Value *val)
{
    setValue(val->getValue());
}
