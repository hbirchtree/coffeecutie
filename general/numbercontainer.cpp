#include "numbercontainer.h"
template<typename T> NumberContainer<T>::NumberContainer(QObject* parent) : QObject(parent)
{
}

template<typename T> NumberContainer<T>::NumberContainer(QObject *parent, T initial) : NumberContainer(parent)
{
    value = initial;
}
template<typename T> NumberContainer<T>::NumberContainer(QPointer<NumberContainer> floater){
    this->setParent(floater->parent());
    bindValue(floater);
}

template<typename T> NumberContainer<T>::~NumberContainer()
{

}

template<typename T> std::function<T()> NumberContainer<T>::getOffsetCallback() const{
    return valueOffsetCallback;
}
template<typename T> T NumberContainer<T>::getRawValue() const{
    return value;
}

template<typename T> T NumberContainer<T>::getValue()
{
    T value = this->value;
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

template<typename T> void NumberContainer<T>::setValueOffsetCallback(std::function<T()> offset){
    valueOffsetCallback = offset;
}

template<typename T> void NumberContainer<T>::setValue(T value)
{
    this->value = value;
}

template<typename T> void NumberContainer<T>::setClamps(T min, T max)
{
    minval = min;
    maxval = max;
}
template<typename T> T NumberContainer<T>::getVelocity() const
{
    return velocity;
}

template<typename T> void NumberContainer<T>::setVelocity(T value)
{
    velocity = value;
}
template<typename T> T NumberContainer<T>::getAcceleration() const
{
    return acceleration;
}

template<typename T> void NumberContainer<T>::setAcceleration(T value)
{
    acceleration = value;
}
