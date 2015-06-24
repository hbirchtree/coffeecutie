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

    std::function<T()> getOffsetCallback() const{
        return valueOffsetCallback;
    }
    T getRawValue() const{
        return value;
    }

    T getValue() const
    {
        T value = this->value;
        if(bound){
            value = bound->getValue();
        }
        value+=valueOffsetCallback();
        return value;
    }

    void setValueOffsetCallback(std::function<T()> &offset){
        valueOffsetCallback = offset;
    }

    void setValue(const T &value)
    {
        unbindValue();
        this->value = value;
    }

    void addValue(const T &value){
        setValue(this->value+value);
    }

    void setClamps(const T &min, const T &max)
    {
        minval = min;
        maxval = max;
    }
    T getVelocity() const
    {
        return velocity;
    }

    void setVelocity(const T &value)
    {
        unbindValue();
        velocity = value;
    }
    T getAcceleration() const
    {
        return acceleration;
    }

    void setAcceleration(const T &value)
    {
        unbindValue();
        acceleration = value;
    }

    void unbindValue(){
        bound = 0;
    }
    void bindValue(NumberContainer<T> *bound){
        this->bound = bound;
    }

private:
    NumberContainer<T> *bound;

    std::function<T()> valueOffsetCallback = [](){
        return T();
    };

    NumberContainer()
    {
        bound = nullptr;
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
