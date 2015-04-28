#include "coffeeomnilight.h"

CoffeeOmniLight::CoffeeOmniLight(QObject *parent) : QObject(parent)
{
    position = new Vector3Container(this);
    color = new Vector3Container(this);
    attenuation = new FloatContainer(this);
    ambientCoefficient = new FloatContainer(this);
}

CoffeeOmniLight::~CoffeeOmniLight()
{

}
QString CoffeeOmniLight::getLightId() const
{
    return lightId;
}

void CoffeeOmniLight::setLightId(const QString &value)
{
    lightId = value;
}
QPointer<Vector3Container> CoffeeOmniLight::getPosition()
{
    return position;
}

void CoffeeOmniLight::setPosition(QPointer<Vector3Container> value)
{
    position = value;
}
QPointer<Vector3Container> CoffeeOmniLight::getColor()
{
    return color;
}

void CoffeeOmniLight::setColor(QPointer<Vector3Container> value)
{
    color = value;
}
QPointer<FloatContainer> CoffeeOmniLight::getAttenuation()
{
    return attenuation;
}

void CoffeeOmniLight::setAttenuation(QPointer<FloatContainer> value)
{
    attenuation = value;
}
QPointer<FloatContainer> CoffeeOmniLight::getAmbientCoefficient()
{
    return ambientCoefficient;
}

void CoffeeOmniLight::setAmbientCoefficient(QPointer<FloatContainer> value)
{
    ambientCoefficient = value;
}






