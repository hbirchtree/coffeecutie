#ifndef QSCRIPTVECTORVALUE_H
#define QSCRIPTVECTORVALUE_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

class Vector3Value : public QObject,public NumberContainer<glm::vec3>
{
public:
    Vector3Value(QObject* parent, const glm::vec3 &initial);
    Vector3Value(QObject* parent, float x, float y, float z);


    using NumberContainer<glm::vec3>::bindValue;
    void bindValue(QPointer<Vector3Value> vec);

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
};

class QuatValue : public QObject,public NumberContainer<glm::quat>
{
public:
    QuatValue(QObject* parent, const glm::quat &initial);
    QuatValue(QObject* parent, float w, float x, float y, float z);

    using NumberContainer<glm::quat>::bindValue;
    void bindValue(QPointer<QuatValue> quat);

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
};

class ScalarValue : public QObject,public NumberContainer<float>
{
public:
    ScalarValue(QObject* parent, float v);

    void bindValue(QPointer<ScalarValue> quat);

    using NumberContainer<float>::operator !=;
    using NumberContainer<float>::operator =;
    using NumberContainer<float>::operator *=;
    using NumberContainer<float>::operator +=;
    using NumberContainer<float>::operator -=;
    using NumberContainer<float>::operator *;
    using NumberContainer<float>::operator +;
    using NumberContainer<float>::operator -;
    using NumberContainer<float>::operator ==;
    using NumberContainer<float>::operator >=;
    using NumberContainer<float>::operator <=;
    using NumberContainer<float>::operator >=;
};

class VectorValue : public QObject
{
    Q_PROPERTY(QVariantList position READ position WRITE setPositionList)
    Q_PROPERTY(QVariantList velocity READ velocity WRITE setVelocityList)
    Q_PROPERTY(QVariantList acceleration READ acceleration WRITE setAccelerationList)

    Q_PROPERTY(float x READ x WRITE setX)
    Q_PROPERTY(float y READ y WRITE setY)
    Q_PROPERTY(float z READ z WRITE setZ)

    Q_OBJECT

public:
    VectorValue(QObject *parent, NumberContainer<glm::vec3> *source);

    float z() const;
    float y() const;
    float x() const;

    QVariantList position() const;
    QVariantList velocity() const;
    QVariantList acceleration() const;

    NumberContainer<glm::vec3>* getRaw();

public slots:
    void bind(VectorValue* v);
    void unbind();

    void setPosition(float x,float y,float z);
    void setVelocity(float x,float y,float z);
    void setAcceleration(float x,float y,float z);

    void setZ(float z);
    void setY(float y);
    void setX(float x);

    void setPositionList(QVariantList position);
    void setVelocityList(QVariantList velocity);
    void setAccelerationList(QVariantList acceleration);

    void pMult(float c);
    void pMult(float x,float y,float z);
    void pAdd(float x,float y,float z);

private:
    NumberContainer<glm::vec3> *source;
};

class QuaternionValue : public QObject
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT

public:
    QuaternionValue(QObject* parent,NumberContainer<glm::quat>* source);
    QVariantList value() const;
    NumberContainer<glm::quat>* getRaw();

public slots:
    void bind(QuaternionValue* v);
    void unbind();
    void setValue(QVariantList value);

private:
    NumberContainer<glm::quat> *source;
};

#endif // QSCRIPTVECTORVALUE_H
