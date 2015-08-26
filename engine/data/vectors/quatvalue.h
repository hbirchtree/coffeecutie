#ifndef QUATVALUE_H
#define QUATVALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class QuatValue : public VectorData,public NumberContainer<glm::quat>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT
public:
    QuatValue(QObject* parent, const glm::quat &initial);
    QuatValue(QObject* parent, std::function<glm::quat(const glm::quat& v)> fun);
    QuatValue(QObject* parent, float w, float x, float y, float z);
    QuatValue(QObject* parent, glm::quat* address);

    using NumberContainer<glm::quat>::operator !=;
    using NumberContainer<glm::quat>::operator =;
    using NumberContainer<glm::quat>::operator *=;
    using NumberContainer<glm::quat>::operator +=;
    using NumberContainer<glm::quat>::operator -=;
    using NumberContainer<glm::quat>::operator *;
    using NumberContainer<glm::quat>::operator +;
    using NumberContainer<glm::quat>::operator -;
    using NumberContainer<glm::quat>::operator ==;
    using NumberContainer<glm::quat>::operator >=;
    using NumberContainer<glm::quat>::operator <=;
    using NumberContainer<glm::quat>::operator >=;
    using NumberContainer<glm::quat>::bindValue;
    using NumberContainer<glm::quat>::attachPointers;

    QVariantList value() const;
    void setValue(const glm::quat& val);

    void unbindValue();

    void updateVectorData();
    uint32_t getVectorDataSize()const;

public slots:
    void mathCumulate(const QVariantList &vals);
    void mathCumulate(QuatValue* val);

    void mathNormalize();

    void bindValue(QuatValue *quat);
    void setValue(const QVariantList &value);
};

#endif // QUATVALUE_H
