#ifndef VECTOR3VALUE_H
#define VECTOR3VALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class Vector3Value : public VectorData,public NumberContainer<glm::vec3>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)
    Q_PROPERTY(QVariantList velocity READ velocity WRITE setVelocity)
    Q_PROPERTY(QVariantList acceleration READ acceleration WRITE setAcceleration)

    Q_OBJECT
public:
    Vector3Value(QObject* parent, const glm::vec3 &initial);
    Vector3Value(QObject* parent, std::function<glm::vec3(const glm::vec3& v)> fun);
    Vector3Value(QObject* parent, float x, float y, float z);

//    using NumberContainer<glm::vec3>::setValue;
    using NumberContainer<glm::vec3>::setVelocity;
    using NumberContainer<glm::vec3>::setAcceleration;
    using NumberContainer<glm::vec3>::operator !=;
    using NumberContainer<glm::vec3>::operator =;
    using NumberContainer<glm::vec3>::operator *=;
    using NumberContainer<glm::vec3>::operator +=;
    using NumberContainer<glm::vec3>::operator -=;
    using NumberContainer<glm::vec3>::operator *;
    using NumberContainer<glm::vec3>::operator +;
    using NumberContainer<glm::vec3>::operator -;
    using NumberContainer<glm::vec3>::operator ==;
    using NumberContainer<glm::vec3>::operator >=;
    using NumberContainer<glm::vec3>::operator <=;
    using NumberContainer<glm::vec3>::operator >=;
    using NumberContainer<glm::vec3>::bindValue;

    QVariantList velocity() const;
    QVariantList value() const;
    QVariantList acceleration() const;

    void setValue(const glm::vec3& val);

    void unbindValue();

    void updateVectorData();
    const void* getVectorData();
    uint32_t getVectorDataSize()const;

signals:
    void valueChanged();
    void velocityChanged();
    void accelerationChanged();

public slots:
    void mathMultiply(const QVariantList &vals);
    void mathMultiply(Vector3Value* val);
    void mathAdd(const QVariantList &vals);
    void mathAdd(Vector3Value* val);
    void mathNormalize();

    void bindValue(Vector3Value *vec);
    void setValue(const QVariantList &value);
    void setVelocity(const QVariantList &velocity);
    void setAcceleration(const QVariantList &value);
};

#endif // VECTOR3VALUE_H
