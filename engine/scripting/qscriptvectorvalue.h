#ifndef QSCRIPTVECTORVALUE_H
#define QSCRIPTVECTORVALUE_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

class VectorValue : public QObject
{
    Q_PROPERTY(QVariantList position READ position WRITE setPositionList)
    Q_PROPERTY(QVariantList velocity READ velocity WRITE setVelocityList)
    Q_PROPERTY(QVariantList acceleration READ acceleration WRITE setAccelerationList)

    Q_PROPERTY(float x READ x WRITE setX)
    Q_PROPERTY(float y READ y WRITE setY)
    Q_PROPERTY(float z READ z WRITE setZ)

    Q_PROPERTY(float velocityX READ velocityX WRITE setVelocityX)
    Q_PROPERTY(float velocityY READ velocityY WRITE setVelocityY)
    Q_PROPERTY(float velocityZ READ velocityZ WRITE setVelocityZ)

    Q_PROPERTY(float accelerationX READ accelerationX WRITE setAccelerationX)
    Q_PROPERTY(float accelerationY READ accelerationY WRITE setAccelerationY)
    Q_PROPERTY(float accelerationZ READ accelerationZ WRITE setAccelerationZ)

    Q_OBJECT

public:
    VectorValue(NumberContainer<glm::vec3>* source);

    float z() const;
    float y() const;
    float x() const;

    float velocityZ() const;
    float velocityY() const;
    float velocityX() const;

    float accelerationX() const;
    float accelerationY() const;
    float accelerationZ() const;

    QVariantList position() const;
    QVariantList velocity() const;
    QVariantList acceleration() const;

public slots:
    void setPosition(float x,float y,float z);
    void setVelocity(float x,float y,float z);
    void setAcceleration(float x,float y,float z);

    void setZ(float z);
    void setY(float y);
    void setX(float x);

    void setVelocityZ(float velocityZ);
    void setVelocityY(float velocityY);
    void setVelocityX(float velocityX);

    void setAccelerationX(float accelerationX);
    void setAccelerationY(float accelerationY);
    void setAccelerationZ(float accelerationZ);

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
    QuaternionValue(NumberContainer<glm::quat>* source);
    QVariantList value() const;
public slots:
    void setValue(QVariantList value);

private:
    NumberContainer<glm::quat> *source;
};

#endif // QSCRIPTVECTORVALUE_H
