#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
class CoffeeOmniLight : public QObject
{
    Q_OBJECT
public:
    CoffeeOmniLight(QObject *parent);
    CoffeeOmniLight(QObject *parent,QString id,glm::vec3 pos,glm::vec3 color,float att,float ambco);
    ~CoffeeOmniLight();

    QString getLightId() const;
    void setLightId(const QString &value);

    QPointer<NumberContainer<glm::vec3>> getPosition();
    void setPosition(QPointer<NumberContainer<glm::vec3>> value);

    QPointer<NumberContainer<glm::vec3>> getColor();
    void setColor(QPointer<NumberContainer<glm::vec3>> value);

    QPointer<NumberContainer<float> > getAttenuation();
    void setAttenuation(QPointer<NumberContainer<float>> value);

    QPointer<NumberContainer<float>> getAmbientCoefficient();
    void setAmbientCoefficient(QPointer<NumberContainer<float>> value);

private:
    QPointer<NumberContainer<glm::vec3>> position;
    QPointer<NumberContainer<glm::vec3>> color;
    QPointer<NumberContainer<float>> attenuation;
    QPointer<NumberContainer<float>> ambientCoefficient;
};

#endif // COFFEEOMNILIGHT_H
