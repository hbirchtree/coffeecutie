#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

class VectorValue;
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

    QPointer<NumberContainer<glm::vec3>> getPosition();

    QPointer<NumberContainer<glm::vec3>> getColor();

    QPointer<NumberContainer<float> > getAttenuation();
    float getAttenuationValue() const;

    QPointer<NumberContainer<float>> getAmbientCoefficient();
    float getAmbientCoefficientValue() const;

    QObject* getColorValue();
    QObject* getPositionValue();

    QObject* getColorVariant();
    QObject* getPositionVariant();
    QObject* getAttenuationVariant();
    QObject* getAmbientCoefficientVariant();

private:
    QPointer<NumberContainer<glm::vec3>> position;
    QPointer<NumberContainer<glm::vec3>> color;
    QPointer<NumberContainer<float>> attenuation;
    QPointer<NumberContainer<float>> ambientCoefficient;

    VectorValue* colorValue;
    VectorValue* positionValue;

    ShaderVariant* colorVariant;
    ShaderVariant* positionVariant;

    ShaderVariant* ambientCoefficientVariant;
    ShaderVariant* attenuationVariant;
};

#endif // COFFEEOMNILIGHT_H
