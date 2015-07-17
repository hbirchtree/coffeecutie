#include "scalarvalue.h"

ScalarValue::ScalarValue(QObject *parent, ScalarDataType v) :
    VectorData(parent),
    NumberContainer<ScalarDataType>(v)
{
}

ScalarValue::ScalarValue(QObject *parent, std::function<ScalarDataType (ScalarDataType)> fun) :
    VectorData(parent),
    NumberContainer<ScalarDataType>(fun)
{

}

ScalarDataType ScalarValue::value() const
{
    return getValue();
}

void ScalarValue::updateVectorData()
{
    if(!_tmp_vec_storage)
        _tmp_vec_storage = malloc(getVectorDataSize());
    ScalarDataType vec = getValue();
    memcpy(_tmp_vec_storage,&vec,getVectorDataSize());
}

const void *ScalarValue::getVectorData()
{
    updateVectorData();
    return _tmp_vec_storage;
}

uint32_t ScalarValue::getVectorDataSize() const
{
    return sizeof(ScalarDataType);
}

void ScalarValue::setValue(ScalarDataType value)
{
    NumberContainer<ScalarDataType>::setValue(value);
    valueChanged();
}

void ScalarValue::bindValue(ScalarValue *val)
{
    boundConnection = connect(val,&ScalarValue::valueChanged,[=](){
        valueChanged();
    });
    NumberContainer<ScalarDataType>::bindValue(((NumberContainer<ScalarDataType>*)val));
}
