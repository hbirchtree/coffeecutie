#ifndef FLOATCONTAINER_H
#define FLOATCONTAINER_H

#include "general/common.h"

class FloatContainer : public QObject
{
    Q_OBJECT
public:
    FloatContainer(QObject* parent);
    FloatContainer(const FloatContainer &floater);
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

    float value;
    float velocity;
    float acceleration;

    float minval;
    float maxval;

    float animationVelocity;
    float animationAcceleration;

};

#endif // FLOATCONTAINER_H
