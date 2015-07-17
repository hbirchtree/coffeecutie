#ifndef VECTOR2VALUE_H
#define VECTOR2VALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class Vector2Value : public VectorData,public NumberContainer<glm::vec2>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT

public:
    Vector2Value(QObject* parent, const glm::vec2 &initial);
    Vector2Value(QObject* parent, std::function<glm::vec2(const glm::vec2& v)> fun);
    Vector2Value(QObject* parent, float x, float y);

    using NumberContainer<glm::vec2>::setVelocity;
    using NumberContainer<glm::vec2>::setAcceleration;
    using NumberContainer<glm::vec2>::operator !=;
    using NumberContainer<glm::vec2>::operator =;
    using NumberContainer<glm::vec2>::operator *=;
    using NumberContainer<glm::vec2>::operator +=;
    using NumberContainer<glm::vec2>::operator -=;
    using NumberContainer<glm::vec2>::operator *;
    using NumberContainer<glm::vec2>::operator +;
    using NumberContainer<glm::vec2>::operator -;
    using NumberContainer<glm::vec2>::operator ==;
    using NumberContainer<glm::vec2>::operator >=;
    using NumberContainer<glm::vec2>::operator <=;
    using NumberContainer<glm::vec2>::operator >=;
    using NumberContainer<glm::vec2>::bindValue;

    void setValue(const glm::vec2 &val);

    QVariantList value() const;

    void unbindValue();

    void updateVectorData();
    const void* getVectorData();
    uint32_t getVectorDataSize()const;
signals:
    void valueChanged();

public slots:
    void setValue(const QVariantList &value);
    void bindValue(Vector2Value *vec);
};

#endif // VECTOR2VALUE_H
