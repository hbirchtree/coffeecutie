#include "coffeeomnilight.h"

CoffeeOmniLight::CoffeeOmniLight(QObject *parent) : QObject(parent)
{
    position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    color = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    attenuation = new NumberContainer<float>(this,0.01f);
    ambientCoefficient = new NumberContainer<float>(this,0.01f);
}

CoffeeOmniLight::CoffeeOmniLight(QObject *parent, QString id, glm::vec3 pos, glm::vec3 color, float att, float ambco) : CoffeeOmniLight(parent)
{
    lightId = id;
    position->setValue(pos);
    this->color->setValue(color);
    attenuation->setValue(att);
    ambientCoefficient->setValue(ambco);
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
QPointer<NumberContainer<glm::vec3>> CoffeeOmniLight::getPosition()
{
    return position;
}

void CoffeeOmniLight::setPosition(QPointer<NumberContainer<glm::vec3>> value)
{
    position = value;
}
QPointer<NumberContainer<glm::vec3>> CoffeeOmniLight::getColor()
{
    return color;
}

void CoffeeOmniLight::setColor(QPointer<NumberContainer<glm::vec3>> value)
{
    color = value;
}
QPointer<NumberContainer<float>> CoffeeOmniLight::getAttenuation()
{
    return attenuation;
}

void CoffeeOmniLight::setAttenuation(QPointer<NumberContainer<float>> value)
{
    attenuation = value;
}
QPointer<NumberContainer<float> > CoffeeOmniLight::getAmbientCoefficient()
{
    return ambientCoefficient;
}

void CoffeeOmniLight::setAmbientCoefficient(QPointer<NumberContainer<float> > value)
{
    ambientCoefficient = value;
}
