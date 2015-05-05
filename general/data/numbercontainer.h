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
    void operator*=(const NumberContainer<T>& t){
        setValue(getValue()*t.getValue());
    }

    T &operator[](int i){
        if(i>2)
            return value;
        else
            switch(i){
            case 0:
                return value;
            case 1:
                return velocity;
            case 2:
                return acceleration;
            }
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

    void addValue(T value){
        setValue(this->value+value);
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
