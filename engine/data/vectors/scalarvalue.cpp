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

ScalarValue::ScalarValue(QObject *parent, float floor, float ceil) :
ScalarValue(parent,[=](ScalarDataType v){
    Q_UNUSED(v)
    return (ceil-floor)*((float)qrand()/(float)RAND_MAX)-(ceil-floor)/2;
})
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

uint32_t ScalarValue::getVectorDataSize() const
{
    return sizeof(ScalarDataType);
}

void ScalarValue::unbindValue()
{
    VectorData::bindSignals(nullptr);
    NumberContainer<ScalarDataType>::unbindValue();
}

void ScalarValue::setValue(ScalarDataType value)
{
    NumberContainer<ScalarDataType>::setValue(value);
    valueChanged();
}

void ScalarValue::bindValue(ScalarValue *val)
{
    VectorData::bindSignals(val);
    NumberContainer<ScalarDataType>::bindValue(((NumberContainer<ScalarDataType>*)val));
}
