#include "coffeeomnilight.h"

#include "engine/scripting/qscriptvectorvalue.h"
#include "general/shadervariant.h"

CoffeeOmniLight::CoffeeOmniLight(QObject *parent) : QObject(parent)
{
    position = new Vector3Value(this,glm::vec3(0,0,0));
    color = new Vector3Value(this,glm::vec3(1,1,1));
    attenuation = new ScalarValue(this,0.01f);
    ambientCoefficient = new ScalarValue(this,0.01f);

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

float CoffeeOmniLight::getAttenuationValue() const
{
    return attenuation->getValue();
}
QPointer<ScalarValue> CoffeeOmniLight::getAmbientCoefficient()
{
    return ambientCoefficient;
}

float CoffeeOmniLight::getAmbientCoefficientValue() const
{
    return ambientCoefficient->getValue();
}

QObject *CoffeeOmniLight::getColorValue()
{
    return color;
}

QObject *CoffeeOmniLight::getPositionValue()
{
    return position;
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

void CoffeeOmniLight::setAttenuationValue(float v)
{
    attenuation->setValue(v);
}

void CoffeeOmniLight::setAmbientCoefficientValue(float v)
{
    ambientCoefficient->setValue(v);
}
