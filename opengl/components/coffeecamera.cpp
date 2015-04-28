#include "coffeecamera.h"

CoffeeCamera::CoffeeCamera(QObject *parent) : QObject(parent)
{
    this->aspect = new FloatContainer(this);
    this->fov = new FloatContainer(this);
    this->position = new Vector3Container(this);
    this->rotation = new Vector3Container(this);
}

CoffeeCamera::CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov) : CoffeeCamera(parent)
{
    this->aspect->setValue(aspect);
    this->fov->setValue(fov);
    this->znear = znear;
    this->zfar = zfar;
}

CoffeeCamera::CoffeeCamera(QObject *pparent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::vec3 rot) : CoffeeCamera(pparent,aspect,znear,zfar,fov)
{
    this->position->setValue(pos);
    this->rotation->setValue(rot);
}

CoffeeCamera::~CoffeeCamera()
{
    aspect->deleteLater();
    fov->deleteLater();
    position->deleteLater();
    rotation->deleteLater();
}

QPointer<Vector3Container> CoffeeCamera::getPosition()
{
    return position;
}

QPointer<Vector3Container> CoffeeCamera::getRotation()
{
    return rotation;
}

QPointer<FloatContainer> CoffeeCamera::getFieldOfView()
{
    return fov;
}

QPointer<FloatContainer> CoffeeCamera::getAspect()
{
    return aspect;
}

void CoffeeCamera::normalizeOrientation()
{
    glm::vec3 value = rotation->getValue();
    glm::vec3 valmax = rotation->getMaxClamp();
    glm::vec3 valmin = rotation->getMinClamp();
    value.y = std::fmod(value.y,360.0f);
    if(value.y<0.0f)
        value.y+=360.0f;

    if(value.x>valmax.x)
        value.x = valmax.x;
    if(value.x<valmin.x)
        value.x = valmin.x;
    rotation->setValue(value);
}

glm::mat4 CoffeeCamera::getOrientation()
{
    glm::mat4 ori;
}
