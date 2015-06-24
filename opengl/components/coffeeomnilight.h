#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"

class ShaderVariant;

class CoffeeOmniLight : public QObject
{
    Q_PROPERTY(QObject* color READ getColorValue)
    Q_PROPERTY(QObject* position READ getPositionValue)
    Q_PROPERTY(QObject* colorVariant READ getColorVariant)
    Q_PROPERTY(QObject* positionVariant READ getPositionVariant)

    Q_PROPERTY(float attenuation READ getAttenuationValue)
    Q_PROPERTY(float ambientCoefficient READ getAmbientCoefficientValue)
    Q_PROPERTY(QObject* attenuationVariant READ getAttenuationVariant)
    Q_PROPERTY(QObject* ambientCoefficientVariant READ getAmbientCoefficientVariant)

    Q_OBJECT
public:
    CoffeeOmniLight(QObject *parent);
    CoffeeOmniLight(QObject *parent,QString id,glm::vec3 pos,glm::vec3 color,float att,float ambco);
    ~CoffeeOmniLight();

    QPointer<Vector3Value> getPosition();

    QPointer<Vector3Value> getColor();

    QPointer<ScalarValue> getAttenuation();
    float getAttenuationValue() const;

    QPointer<ScalarValue> getAmbientCoefficient();
    float getAmbientCoefficientValue() const;

    QObject* getColorValue();
    QObject* getPositionValue();

    QObject* getColorVariant();
    QObject* getPositionVariant();
    QObject* getAttenuationVariant();
    QObject* getAmbientCoefficientVariant();

private:
    QPointer<Vector3Value> position;
    QPointer<Vector3Value> color;
    QPointer<ScalarValue> attenuation;
    QPointer<ScalarValue> ambientCoefficient;

    ShaderVariant* colorVariant;
    ShaderVariant* positionVariant;

    ShaderVariant* ambientCoefficientVariant;
    ShaderVariant* attenuationVariant;
};

#endif // COFFEEOMNILIGHT_H
