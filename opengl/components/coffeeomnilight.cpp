#include "coffeeomnilight.h"

#include "engine/scripting/qscriptvectorvalue.h"

CoffeeOmniLight::CoffeeOmniLight(QObject *parent) : QObject(parent)
{
    position = new Vector3Value(this,glm::vec3(0,0,0));
    color = new Vector3Value(this,glm::vec3(1,1,1));
    attenuation = new ScalarValue(this,0.01f);
    ambientCoefficient = new ScalarValue(this,0.01f);
}

CoffeeOmniLight::CoffeeOmniLight(QObject *parent, QString id, glm::vec3 pos, glm::vec3 color, float att, float ambco) : CoffeeOmniLight(parent)
{
    this->setObjectName(id);
    position->setValue(pos);
    this->color->setValue(color);
    attenuation->setValue(att);
    ambientCoefficient->setValue(ambco);
}

CoffeeOmniLight::~CoffeeOmniLight()
{
}

QPointer<Vector3Value> CoffeeOmniLight::getPosition()
{
    return position;
}

QPointer<Vector3Value> CoffeeOmniLight::getColor()
{
    return color;
}

QPointer<ScalarValue> CoffeeOmniLight::getAttenuation()
{
    return attenuation;
}

QPointer<ScalarValue> CoffeeOmniLight::getAmbientCoefficient()
{
    return ambientCoefficient;
}

QObject *CoffeeOmniLight::getColorValue()
{
    return color;
}

QObject *CoffeeOmniLight::getPositionValue()
{
    return position;
}

QObject *CoffeeOmniLight::getAttenuationVariant()
{
    return attenuation;
}

QObject *CoffeeOmniLight::getAmbientCoefficientVariant()
{
    return ambientCoefficient;
}
