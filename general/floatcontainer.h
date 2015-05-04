#ifndef FLOATCONTAINER_H
#define FLOATCONTAINER_H

#include "general/common.h"

class FloatContainer : public QObject
{
    Q_OBJECT
public:
    FloatContainer(QObject* parent);
    FloatContainer(QObject* parent,float initial);
    FloatContainer(QObject *parent, const FloatContainer &floater);
    FloatContainer(QPointer<FloatContainer> floater);
    ~FloatContainer();

    float getValue();
    void setValue(float value);

    void setClamps(float min,float max);

    float getVelocity() const;
    void setVelocity(float value);

    float getAcceleration() const;
    void setAcceleration(float value);

    void unbindValue(){
        bound = 0;
    }
    void bindValue(QPointer<FloatContainer> bound){
        this->bound = bound;
    }
    void setValueOffsetCallback(std::function<float()> offset);

    std::function<float()> getOffsetCallback() const;
    float getRawValue() const;

signals:

public slots:

private:
    QPointer<FloatContainer> bound;
    std::function<float()> valueOffsetCallback = [](){
        return 0.0f;
    };

    float value = 0;
    float velocity = 0;
    float acceleration = 0;

    float minval = 0;
    float maxval = 0;

    float animationVelocity = 0;
    float animationAcceleration = 0;

};

#endif // FLOATCONTAINER_H
