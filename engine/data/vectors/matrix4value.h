#ifndef MATRIX4VALUE_H
#define MATRIX4VALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class Matrix4Value : public VectorData,public NumberContainer<glm::mat4>
{
    Q_PROPERTY(QVariantMap values READ values)

    Q_OBJECT

public:
    Matrix4Value(QObject* parent);
    Matrix4Value(QObject* parent, std::function<glm::mat4(const glm::mat4& v)> fun);

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

    void unbindValue();

    void updateVectorData();
    const void* getVectorData();
    uint32_t getVectorDataSize()const;
    QVariantMap values() const;

signals:
    void valueChanged();

public slots:
    void setValue(const glm::mat4& val);
    void setValue(Matrix4Value *val);
};

#endif // MATRIX4VALUE_H
