#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"

class Vector3Value;
class ScalarValue;

class CoffeeOmniLight : public QObject
{
    Q_PROPERTY(QObject* color READ getColorValue)
    Q_PROPERTY(QObject* position READ getPositionValue)

    Q_PROPERTY(QObject* attenuation READ getAttenuationVariant)
    Q_PROPERTY(QObject* ambientCoefficient READ getAmbientCoefficientVariant)

    Q_OBJECT
public:
    CoffeeOmniLight(QObject *parent);
    CoffeeOmniLight(QObject *parent,QString id,glm::vec3 pos,glm::vec3 color,float att,float ambco);
    ~CoffeeOmniLight();

    QPointer<Vector3Value> getPosition();

    QPointer<Vector3Value> getColor();

    QPointer<ScalarValue> getAttenuation();

    QPointer<ScalarValue> getAmbientCoefficient();

    QObject* getColorValue();
    QObject* getPositionValue();

    QObject* getAttenuationVariant();
    QObject* getAmbientCoefficientVariant();

private:
    QPointer<Vector3Value> position;
    QPointer<Vector3Value> color;
    QPointer<ScalarValue> attenuation;
    QPointer<ScalarValue> ambientCoefficient;
};

#endif // COFFEEOMNILIGHT_H
