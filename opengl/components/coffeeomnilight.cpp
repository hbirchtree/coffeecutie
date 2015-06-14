#include "coffeeomnilight.h"

#include "engine/scripting/qscriptvectorvalue.h"
#include "general/shadervariant.h"

CoffeeOmniLight::CoffeeOmniLight(QObject *parent) : QObject(parent)
{
    position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    color = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    attenuation = new NumberContainer<float>(this,0.01f);
    ambientCoefficient = new NumberContainer<float>(this,0.01f);

    this->colorValue = new VectorValue(color);
    this->positionValue = new VectorValue(position);

    this->colorVariant = new ShaderVariant([=](){
        return this->color->getValue();
    });
    this->positionVariant = new ShaderVariant([=](){
        return this->position->getValue();
    });

    this->ambientCoefficientVariant = new ShaderVariant([=](){
        return this->getAmbientCoefficientValue();
    });
    this->attenuationVariant = new ShaderVariant([=](){
        return this->getAttenuationValue();
    });
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
    delete colorVariant;
    delete positionVariant;
    delete ambientCoefficientVariant;
    delete attenuationVariant;
}
QPointer<NumberContainer<glm::vec3>> CoffeeOmniLight::getPosition()
{
    return position;
}

QPointer<NumberContainer<glm::vec3>> CoffeeOmniLight::getColor()
{
    return color;
}

QPointer<NumberContainer<float>> CoffeeOmniLight::getAttenuation()
{
    return attenuation;
}

float CoffeeOmniLight::getAttenuationValue() const
{
    return attenuation->getValue();
}
QPointer<NumberContainer<float> > CoffeeOmniLight::getAmbientCoefficient()
{
    return ambientCoefficient;
}

float CoffeeOmniLight::getAmbientCoefficientValue() const
{
    return ambientCoefficient->getValue();
}

QObject *CoffeeOmniLight::getColorValue()
{
    return colorValue;
}

QObject *CoffeeOmniLight::getPositionValue()
{
    return positionValue;
}

QObject *CoffeeOmniLight::getColorVariant()
{
    return colorVariant;
}

QObject *CoffeeOmniLight::getPositionVariant()
{
    return positionVariant;
}

QObject *CoffeeOmniLight::getAttenuationVariant()
{
    return attenuationVariant;
}

QObject *CoffeeOmniLight::getAmbientCoefficientVariant()
{
    return ambientCoefficientVariant;
}
