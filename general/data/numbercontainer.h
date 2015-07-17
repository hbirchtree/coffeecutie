#ifndef NUMBERCONTAINER_H
#define NUMBERCONTAINER_H

#include "general/common.h"

template<class T> class NumberContainer
{
public:
    NumberContainer(T initial) : NumberContainer()
    {
        value = initial;
    }
    NumberContainer(const std::function<T(const T&)>& func) : NumberContainer()
    {
        valueTransform = func;
    }

    NumberContainer(NumberContainer<T> *floater) : NumberContainer(){
        bindValue(floater);
    }
    ~NumberContainer()
    {
    }

    T operator+(const T& t){
        return getValue()+t;
    }
    T operator+(const NumberContainer<T>& t){
        return getValue()+t.getValue();
    }
    T operator-(const T& t){
        return getValue()-t;
    }
    T operator-(const NumberContainer<T>& t){
        return getValue()-t.getValue();
    }
    T operator*(const T& t){
        return getValue()*t;
    }
    T operator*(float t){
        return getValue()*t;
    }
    T operator*(const NumberContainer<T>& t){
        return getValue()*t.getValue();
    }

    T operator()(){
        return getValue();
    }
    T operator()(int i){
        switch(i){
        case 1:
            return getVelocity();
        case 2:
            return getAcceleration();
        default:
            return getValue();
        }
    }

    bool operator==(const T& t){
        return t==getValue();
    }
    bool operator==(const NumberContainer<T>& t){
        return t.getValue()==getValue();
    }
    bool operator<=(const T& t){
        return t<=getValue();
    }
    bool operator<=(const NumberContainer<T>& t){
        return t.getValue()<=getValue();
    }
    bool operator>=(const T& t){
        return t>=getValue();
    }
    bool operator>=(const NumberContainer<T>& t){
        return t.getValue()>=getValue();
    }
    bool operator!=(const T& t){
        return t!=getValue();
    }
    bool operator!=(const NumberContainer<T>& t){
        return t.getValue()!=getValue();
    }

    void operator=(const T& t){
        setValue(t);
    }
    void operator=(const NumberContainer<T>& t){
        setValue(t.getValue());
    }
    void operator=(NumberContainer<T> *t){
        bindValue(t);
    }
    void operator+=(const T& t){
        setValue(getValue()+t);
    }
    void operator+=(const NumberContainer<T>& t){
        setValue(getValue()+t.getValue());
    }
    void operator-=(const T& t){
        setValue(getValue()-t);
    }
    void operator-=(const NumberContainer<T>& t){
        setValue(getValue()-t.getValue());
    }
    void operator*=(const T& t){
        setValue(getValue()*t);
    }
    void operator*=(float t){
        setValue(getValue()*t);
    }
    void operator*=(const NumberContainer<T>& t){
        setValue(getValue()*t.getValue());
    }

    T &operator[](int i){
        switch(i){
        case 1:
            return velocity;
        case 2:
            return acceleration;
        default:
            return value;
        }
    }

    virtual T getRawValue() const{
        return value;
    }

    virtual T getValue() const
    {
        T value = this->value;
        if(bound){
            value = bound->getValue();
        }
        value = valueTransform(value);
        return value;
    }

    virtual void setValue(const T &value)
    {
        unbindValue();
        this->value = value;
    }

    virtual void addValue(const T &value){
        setValue(this->value+value);
    }

    void setClamps(const T &min, const T &max)
    {
        minval = min;
        maxval = max;
    }
    virtual T getVelocity() const
    {
        return velocity;
    }

    virtual void setVelocity(const T &value)
    {
        unbindValue();
        velocity = value;
    }
    virtual T getAcceleration() const
    {
        return acceleration;
    }

    virtual void setAcceleration(const T &value)
    {
        unbindValue();
        acceleration = value;
    }

    virtual void unbindValue(){
        bound = nullptr;
    }
    virtual void bindValue(NumberContainer<T> *bound){
        this->bound = bound;
    }

    std::function<T (const T &)> getValueTransform() const
    {
        return valueTransform;
    }
    void setValueTransform(const std::function<T (const T &)> &value)
    {
        valueTransform = value;
    }

protected:
    QMetaObject::Connection boundConnection;

private:
    NumberContainer<T> *bound = nullptr;

    std::function<T(const T&)> valueTransform = [](const T& v){
        return v;
    };

    NumberContainer()
    {
    }

    T value;
    T velocity;
    T acceleration;

    T minval;
    T maxval;

    T animationVelocity;
    T animationAcceleration;
};

#endif // NUMBERCONTAINER_H
