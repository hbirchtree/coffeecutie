#include "floatcontainer.h"

FloatContainer::FloatContainer()
{

}
FloatContainer::FloatContainer(QPointer<FloatContainer> floater){
    bindValue(floater);
}

FloatContainer::~FloatContainer()
{

}
float FloatContainer::getValue()
{
    float value = this->value;
    if(bound){
        value = bound->getValue();
    }else
        unbindValue();
    value+=valueOffsetCallback();
    return value;
}

void FloatContainer::setValue(float value)
{
    value = value;
}
float FloatContainer::getVelocity()
{
    return velocity;
}

void FloatContainer::setVelocity(float value)
{
    velocity = value;
}
float FloatContainer::getAcceleration()
{
    return acceleration;
}

void FloatContainer::setAcceleration(float value)
{
    acceleration = value;
}




