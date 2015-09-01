#ifndef VECTOR4VALUE_H
#define VECTOR4VALUE_H

#include "engine/scripting/qscriptvectorvalue.h"
#include <QColor>

class Vector4Value : public VectorData,public NumberContainer<glm::vec4>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)
    Q_PROPERTY(QColor color READ asColor)

    Q_OBJECT

public:
    Vector4Value(QObject* parent, const glm::vec4 &initial);
    Vector4Value(QObject* parent, std::function<glm::vec4(const glm::vec4& v)> fun);

    using NumberContainer<glm::vec4>::setVelocity;
    using NumberContainer<glm::vec4>::setAcceleration;
    using NumberContainer<glm::vec4>::operator !=;
    using NumberContainer<glm::vec4>::operator =;
    using NumberContainer<glm::vec4>::operator *=;
    using NumberContainer<glm::vec4>::operator +=;
    using NumberContainer<glm::vec4>::operator -=;
    using NumberContainer<glm::vec4>::operator *;
    using NumberContainer<glm::vec4>::operator +;
    using NumberContainer<glm::vec4>::operator -;
    using NumberContainer<glm::vec4>::operator ==;
    using NumberContainer<glm::vec4>::operator >=;
    using NumberContainer<glm::vec4>::operator <=;
    using NumberContainer<glm::vec4>::operator >=;

    void setValue(const glm::vec4& val);
    QVariantList value() const;

    void unbindValue();

    void updateVectorData();
    uint32_t getVectorDataSize()const;

    Q_INVOKABLE QColor asColor();

public slots:
    void fromColor(const QColor& color);

    void setValue(const QVariantList &value);
    void bindValue(Vector4Value *vec);
};

#endif // VECTOR4VALUE_H
