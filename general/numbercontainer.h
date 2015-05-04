#ifndef NUMBERCONTAINER_H
#define NUMBERCONTAINER_H

#include <QObject>
template<class T> class NumberContainer : public QObject
{
public:
    NumberContainer(QObject* parent,T initial) : NumberContainer(parent)
    {
        value = initial;
    }
    NumberContainer(QObject* parent,QPointer<NumberContainer<T> > floater) : NumberContainer(parent){
        this->setParent(floater->parent());
        bindValue(floater);
    }
    ~NumberContainer()
    {
    }

    std::function<T()> getOffsetCallback() const{
        return valueOffsetCallback;
    }
    T getRawValue() const{
        return value;
    }

    T getValue()
    {
        T value = this->value;
        if(bound){
            value = bound->getValue();
        }else
            unbindValue();
        value+=valueOffsetCallback();
//        if(minval<maxval&&value>maxval)
//            value=maxval;
//        else if(minval<maxval&&value<minval)
//            value=minval;
        return value;
    }

    void setValueOffsetCallback(std::function<T()> offset){
        valueOffsetCallback = offset;
    }

    void setValue(T value)
    {
        unbindValue();
        this->value = value;
    }

    void setClamps(T min, T max)
    {
        minval = min;
        maxval = max;
    }
    T getVelocity() const
    {
        return velocity;
    }

    void setVelocity(T value)
    {
        unbindValue();
        velocity = value;
    }
    T getAcceleration() const
    {
        return acceleration;
    }

    void setAcceleration(T value)
    {
        unbindValue();
        acceleration = value;
    }

    void unbindValue(){
        bound = 0;
    }
    void bindValue(QPointer<NumberContainer<T> > bound){
        this->bound = bound;
    }

private:
    QPointer<NumberContainer<T> > bound;

    std::function<T()> valueOffsetCallback = [](){
        return T();
    };

    NumberContainer(QObject* parent) : QObject(parent)
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
