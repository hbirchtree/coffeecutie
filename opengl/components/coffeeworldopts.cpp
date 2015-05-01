#include "coffeeworldopts.h"

CoffeeWorldOpts::CoffeeWorldOpts(CoffeeRenderer *renderer) : QObject(renderer)
{
    fogColor.g=1;
    this->renderer = renderer;
}

CoffeeWorldOpts::~CoffeeWorldOpts()
{

}
QPointer<CoffeeCamera> CoffeeWorldOpts::getCamera()
{
    return camera;
}

void CoffeeWorldOpts::setCamera(QPointer<CoffeeCamera> value)
{
    camera = value;
}

void CoffeeWorldOpts::addLight(QPointer<CoffeeOmniLight> light)
{
    lights.append(light);
}

QList<QPointer<CoffeeOmniLight> > CoffeeWorldOpts::getLights()
{
    return lights;
}
glm::vec4 CoffeeWorldOpts::getFogColor() const
{
    return fogColor;
}

void CoffeeWorldOpts::setFogColor(const glm::vec4 &value)
{
    fogColor = value;
}
float CoffeeWorldOpts::getFogDensity() const
{
    return fogDensity;
}

void CoffeeWorldOpts::setFogDensity(float value)
{
    fogDensity = value;
}
QPointer<CoffeeRenderer> CoffeeWorldOpts::getRenderer()
{
    return renderer;
}

void CoffeeWorldOpts::setRenderer(const QPointer<CoffeeRenderer> &value)
{
    renderer = value;
}



