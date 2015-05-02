#ifndef NUMBERCONTAINER_H
#define NUMBERCONTAINER_H

#include <QObject>
template<class T> class NumberContainer
{
public:
    NumberContainer(QObject* parent);
    NumberContainer(QObject* parent,T initial);
    NumberContainer(QPointer<NumberContainer> floater);
    ~NumberContainer();

    T getValue();
    void setValue(T value);

    void setClamps(T min,T max);

    T getVelocity() const;
    void setVelocity(T value);

    T getAcceleration() const;
    void setAcceleration(T value);

    void unbindValue(){
        bound = 0;
    }
    void bindValue(QPointer<NumberContainer> bound){
        this->bound = bound;
    }
    void setValueOffsetCallback(std::function<T()> offset);

    std::function<T()> getOffsetCallback() const;
    T getRawValue() const;

private:
    QPointer<NumberContainer> bound;
    std::function<T()> valueOffsetCallback = [](){
        return 0.0f;
    };

    T value;
    T velocity;
    T acceleration;

    T minval;
    T maxval;

    T animationVelocity;
    T animationAcceleration;
};

#endif // NUMBERCONTAINER_H
