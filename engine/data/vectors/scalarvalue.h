#ifndef SCALARVALUE_H
#define SCALARVALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class ScalarValue : public VectorData,public NumberContainer<ScalarDataType>
{
    Q_PROPERTY(ScalarDataType value READ value WRITE setValue)

    Q_OBJECT

public:
    ScalarValue(QObject* parent, ScalarDataType v);
    ScalarValue(QObject* parent, std::function<ScalarDataType(ScalarDataType v)> fun);

    using NumberContainer<ScalarDataType>::operator !=;
    using NumberContainer<ScalarDataType>::operator =;
    using NumberContainer<ScalarDataType>::operator *=;
    using NumberContainer<ScalarDataType>::operator +=;
    using NumberContainer<ScalarDataType>::operator -=;
    using NumberContainer<ScalarDataType>::operator *;
    using NumberContainer<ScalarDataType>::operator +;
    using NumberContainer<ScalarDataType>::operator -;
    using NumberContainer<ScalarDataType>::operator ==;
    using NumberContainer<ScalarDataType>::operator >=;
    using NumberContainer<ScalarDataType>::operator <=;
    using NumberContainer<ScalarDataType>::operator >=;
    ScalarDataType value() const;

    void updateVectorData();
    const void* getVectorData();
    uint32_t getVectorDataSize()const;
signals:
    void valueChanged();

public slots:
    void setValue(ScalarDataType value);
    void bindValue(ScalarValue *val);
};

#endif // SCALARVALUE_H
