#ifndef FLOATCONTAINER_H
#define FLOATCONTAINER_H

#include <QObject>
#include <QPointer>

class FloatContainer : public QObject
{
    Q_OBJECT
public:
    FloatContainer();
    FloatContainer(QPointer<FloatContainer> floater);
    ~FloatContainer();

    float getValue();
    void setValue(float value);

    float getVelocity();
    void setVelocity(float value);

    float getAcceleration();
    void setAcceleration(float value);

    void unbindValue(){
        bound = 0;
    }
    void bindValue(QPointer<FloatContainer> bound){
        this->bound = bound;
    }

signals:

public slots:

private:
    QPointer<FloatContainer> bound;
    std::function<float()> valueOffsetCallback = [](){
        return 0.0f;
    };

    float value;
    float velocity;
    float acceleration;

    float animationVelocity;
    float animationAcceleration;

};

#endif // FLOATCONTAINER_H
