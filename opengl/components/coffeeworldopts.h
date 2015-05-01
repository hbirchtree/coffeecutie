#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H

#include "general/common.h"
#include "coffeecamera.h"
#include "coffeeomnilight.h"
#include "opengl/coffeerenderer.h"

class CoffeeWorldOpts : public QObject
{
    //An OpenGL-level representation of the world or scene, not meant for modification. That will be done elsewhere.
    Q_OBJECT
public:
    CoffeeWorldOpts(CoffeeRenderer* renderer);
    ~CoffeeWorldOpts();

    QPointer<CoffeeCamera> getCamera();
    void setCamera(QPointer<CoffeeCamera> value);

    void addLight(QPointer<CoffeeOmniLight> light);
    QList<QPointer<CoffeeOmniLight> > getLights();

    glm::vec4 getFogColor() const;
    void setFogColor(const glm::vec4 &value);

    float getFogDensity() const;
    void setFogDensity(float value);

    QPointer<CoffeeRenderer> getRenderer();
    void setRenderer(const QPointer<CoffeeRenderer> &value);

private:
    float fogDensity = 0.01;
    glm::vec4 fogColor;

    QPointer<CoffeeCamera> camera;
    QList<QPointer<CoffeeOmniLight> > lights;
    QPointer<CoffeeRenderer> renderer;
};

#endif // COFFEEWORLDOPTS_H
