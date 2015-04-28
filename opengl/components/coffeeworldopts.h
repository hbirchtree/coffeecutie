#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H

#include "general/common.h"
#include "coffeecamera.h"
#include "coffeeomnilight.h"

class CoffeeWorldOpts : public QObject
{
    //An OpenGL-level representation of the world or scene, not meant for modification. That will be done elsewhere.
    Q_OBJECT
public:
    CoffeeWorldOpts(QObject *parent);
    ~CoffeeWorldOpts();

    QPointer<CoffeeCamera> getCamera() const;
    void setCamera(QPointer<CoffeeCamera> value);

    void addLight(QPointer<CoffeeOmniLight> light);
    QList<QPointer<CoffeeOmniLight> > getLights() const;

    glm::vec4 getFogColor() const;
    void setFogColor(const glm::vec4 &value);

    float getFogDensity() const;
    void setFogDensity(float value);

private:
    float fogDensity = 0.01;
    glm::vec4 fogColor;

    QPointer<CoffeeCamera> camera;
    QList<QPointer<CoffeeOmniLight> > lights;
};

#endif // COFFEEWORLDOPTS_H
