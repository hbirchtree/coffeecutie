#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
class CoffeeOmniLight : public QObject
{
    Q_PROPERTY(QString color READ getStringColor)
    Q_PROPERTY(QString position READ getStringPosition)
    Q_PROPERTY(float attenuation READ getAttenuationValue)
    Q_PROPERTY(float ambientCoefficient READ getAmbientCoefficientValue)

    Q_OBJECT
public:
    CoffeeOmniLight(QObject *parent);
    CoffeeOmniLight(QObject *parent,QString id,glm::vec3 pos,glm::vec3 color,float att,float ambco);
    ~CoffeeOmniLight();

    QPointer<NumberContainer<glm::vec3>> getPosition();
    void setPosition(QPointer<NumberContainer<glm::vec3>> value);
    QString getStringPosition() const;

    QPointer<NumberContainer<glm::vec3>> getColor();
    void setColor(QPointer<NumberContainer<glm::vec3>> value);
    QString getStringColor() const;

    QPointer<NumberContainer<float> > getAttenuation();
    void setAttenuation(QPointer<NumberContainer<float>> value);
    float getAttenuationValue() const;

    QPointer<NumberContainer<float>> getAmbientCoefficient();
    void setAmbientCoefficient(QPointer<NumberContainer<float>> value);
    float getAmbientCoefficientValue() const;

private:
    QPointer<NumberContainer<glm::vec3>> position;
    QPointer<NumberContainer<glm::vec3>> color;
    QPointer<NumberContainer<float>> attenuation;
    QPointer<NumberContainer<float>> ambientCoefficient;
};

#endif // COFFEEOMNILIGHT_H
