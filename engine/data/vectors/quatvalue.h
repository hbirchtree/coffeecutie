#ifndef QUATVALUE_H
#define QUATVALUE_H

#include "engine/scripting/qscriptvectorvalue.h"

class QuatValue : public QObject,public NumberContainer<glm::quat>
{
    Q_PROPERTY(QVariantList value READ value WRITE setValue)

    Q_OBJECT
public:
    QuatValue(QObject* parent, const glm::quat &initial);
    QuatValue(QObject* parent, std::function<glm::quat(const glm::quat& v)> fun);
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

#endif // QUATVALUE_H
