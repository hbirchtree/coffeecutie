#include "floatcontainer.h"
#include <QDebug>

FloatContainer::FloatContainer(QObject* parent) : QObject(parent)
{

}

FloatContainer::FloatContainer(QObject *parent, float initial) : FloatContainer(parent)
{
    value = initial;
}

FloatContainer::FloatContainer(QObject*parent, const FloatContainer &floater) : FloatContainer(parent)
{
    this->value = floater.getRawValue();
    this->velocity = floater.getVelocity();
    this->acceleration = floater.getAcceleration();
    this->valueOffsetCallback = floater.getOffsetCallback();
}
FloatContainer::FloatContainer(QPointer<FloatContainer> floater){
    this->setParent(floater->parent());
    bindValue(floater);
}

FloatContainer::~FloatContainer()
{

}

std::function<float()> FloatContainer::getOffsetCallback() const{
    return valueOffsetCallback;
}
float FloatContainer::getRawValue() const{
    return value;
}

float FloatContainer::getValue()
{
    float value = this->value;
    if(bound){
        value = bound->getValue();
    }else
        unbindValue();
    value+=valueOffsetCallback();
    if(minval<maxval&&value>maxval)
        value=maxval;
    else if(minval<maxval&&value<minval)
        value=minval;
    return value;
}

void FloatContainer::setValueOffsetCallback(std::function<float()> offset){
    valueOffsetCallback = offset;
}

void FloatContainer::setValue(float value)
{
    this->value = value;
}

void FloatContainer::setClamps(float min, float max)
{
    minval = min;
    maxval = max;
}
float FloatContainer::getVelocity() const
{
    return velocity;
}

void FloatContainer::setVelocity(float value)
{
    velocity = value;
}
float FloatContainer::getAcceleration() const
{
    return acceleration;
}

void FloatContainer::setAcceleration(float value)
{
    acceleration = value;
}
