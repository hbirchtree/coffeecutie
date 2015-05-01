#ifndef COFFEEOMNILIGHT_H
#define COFFEEOMNILIGHT_H

#include "general/common.h"
#include "general/vectors/vector3container.h"
#include "general/floatcontainer.h"
class CoffeeOmniLight : public QObject
{
    Q_OBJECT
public:
    CoffeeOmniLight(QObject *parent);
    CoffeeOmniLight(QObject *parent,QString id,glm::vec3 pos,glm::vec3 color,float att,float ambco);
    ~CoffeeOmniLight();

    QString getLightId() const;
    void setLightId(const QString &value);

    QPointer<Vector3Container> getPosition();
    void setPosition(QPointer<Vector3Container> value);

    QPointer<Vector3Container> getColor();
    void setColor(QPointer<Vector3Container> value);

    QPointer<FloatContainer> getAttenuation();
    void setAttenuation(QPointer<FloatContainer> value);

    QPointer<FloatContainer> getAmbientCoefficient();
    void setAmbientCoefficient(QPointer<FloatContainer> value);

private:
    QString lightId;
    QPointer<Vector3Container> position;
    QPointer<Vector3Container> color;
    QPointer<FloatContainer> attenuation;
    QPointer<FloatContainer> ambientCoefficient;
};

#endif // COFFEEOMNILIGHT_H
