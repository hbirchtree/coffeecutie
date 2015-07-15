#ifndef QSCRIPTVECTORVALUE_H
#define QSCRIPTVECTORVALUE_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

class VectorData
{
public:
    virtual void updateVectorData() = 0;
    virtual const void* getVectorData() = 0;
    virtual uint32_t getVectorDataSize() = 0;

protected:
    void* _tmp_vec_storage = nullptr;
};

class Vector3Value : public QObject,public NumberContainer<glm::vec3>,public VectorData
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)
    Q_PROPERTY(QVariantList velocity READ velocity WRITE setVelocity)
    Q_PROPERTY(QVariantList acceleration READ acceleration WRITE setAcceleration)

    Q_OBJECT
public:
    Vector3Value(QObject* parent, const glm::vec3 &initial);
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

    void updateVectorData();
    const void* getVectorData();
    uint32_t getVectorDataSize();

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

class Vector2Value : public QObject, public NumberContainer<glm::vec2>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT

public:
    Vector2Value(QObject* parent, const glm::vec2 &initial);
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

    void setValue(const glm::vec2& val);

    QVariantList value() const;
signals:
    void valueChanged();

public slots:
    void setValue(const QVariantList &value);
    void bindValue(Vector2Value *vec);
};

class QuatValue : public QObject,public NumberContainer<glm::quat>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT
public:
    QuatValue(QObject* parent, const glm::quat &initial);
    QuatValue(QObject* parent, float w, float x, float y, float z);

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

    QVariantList value() const;
    void setValue(const glm::quat& val);

signals:
    void valueChanged();

public slots:
    void mathCumulate(const QVariantList &vals);
    void mathCumulate(QuatValue* val);

    void mathNormalize();

    void bindValue(QuatValue *quat);
    void setValue(const QVariantList &value);
};

typedef float ScalarDataType;

class ScalarValue : public QObject,public NumberContainer<ScalarDataType>
{
    Q_PROPERTY(ScalarDataType value READ value WRITE setValue)

    Q_OBJECT

public:
    ScalarValue(QObject* parent, ScalarDataType v);


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

signals:
    void valueChanged();

public slots:
    void setValue(ScalarDataType value);
    void bindValue(ScalarValue *val);
};

class Matrix4Value : public QObject, public NumberContainer<glm::mat4>
{
    Q_OBJECT
public:
    Matrix4Value(QObject* parent);

    using NumberContainer<glm::mat4>::getValue;
    using NumberContainer<glm::mat4>::setValue;
    using NumberContainer<glm::mat4>::operator !=;
    using NumberContainer<glm::mat4>::operator =;
    using NumberContainer<glm::mat4>::operator *=;
    using NumberContainer<glm::mat4>::operator +=;
    using NumberContainer<glm::mat4>::operator -=;
    using NumberContainer<glm::mat4>::operator *;
    using NumberContainer<glm::mat4>::operator +;
    using NumberContainer<glm::mat4>::operator -;
    using NumberContainer<glm::mat4>::operator ==;
    using NumberContainer<glm::mat4>::operator >=;
    using NumberContainer<glm::mat4>::operator <=;
    using NumberContainer<glm::mat4>::operator >=;
    using NumberContainer<glm::mat4>::bindValue;

    Q_INVOKABLE float getValue(uint col, uint row);

signals:
    void valueChanged();

public slots:
    void setValue(const glm::mat4& val);
    void setValue(Matrix4Value *val);
};

#endif // QSCRIPTVECTORVALUE_H
