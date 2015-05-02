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

CoffeeCamera::CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::vec3 rot) : CoffeeCamera(parent,aspect,znear,zfar,fov)
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

void CoffeeCamera::offsetOrientation(float rightAngle, float upAngle)
{
    glm::vec3 curr = rotation->getValue();
    curr.y+=rightAngle;
    curr.x+=upAngle;
    rotation->setValue(curr);
    normalizeOrientation();
}

void CoffeeCamera::cameraLookAt(glm::vec3 point)
{
    glm::lookAt(position->getValue(),point,glm::vec3(0,1,0));
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

glm::vec3 CoffeeCamera::getCameraRight() const
{
    return glm::vec3(glm::inverse(getOrientationMatrix()) * glm::vec4(1,0,0,1));
}
glm::vec3 CoffeeCamera::getCameraUp() const
{
    return glm::vec3(glm::inverse(getOrientationMatrix()) * glm::vec4(0,1,0,1));
}
glm::vec3 CoffeeCamera::getCameraForward() const
{
    return glm::vec3(glm::inverse(getOrientationMatrix()) * glm::vec4(0,0,-1,1));
}

glm::vec3 CoffeeCamera::getCameraRightNormal() const{
    return glm::normalize(getCameraRight());
}
glm::vec3 CoffeeCamera::getCameraUpNormal() const{
    return glm::normalize(getCameraUp());
}
glm::vec3 CoffeeCamera::getCameraForwardNormal() const{
    return glm::normalize(getCameraForward());
}

glm::mat4 CoffeeCamera::getOrientationMatrix() const
{
    glm::mat4 ori;
    ori = glm::rotate(ori,rotation->getValue().x*math_pi/180.f,glm::vec3(1,0,0));
    ori = glm::rotate(ori,rotation->getValue().y*math_pi/180.f,glm::vec3(0,1,0));
    return ori;
}

glm::mat4 CoffeeCamera::getProjection() const
{
    glm::mat4 camera = glm::perspective(fov->getValue()*math_pi/180.f,aspect->getValue(),znear,zfar);
//    camera *= glm::lookAt(position->getValue(),glm::vec3(0,0,0),glm::vec3(0,1,0));
    camera = glm::translate(camera,-position->getValue());
    camera *= getOrientationMatrix();
    return camera;
}

glm::mat4 CoffeeCamera::getOrthographic() const
{
    if(framebufferSize==NULL)
        return glm::mat4();
    glm::mat4 camera = glm::ortho(0,framebufferSize->width(),0,framebufferSize->height());
    camera *= getOrientationMatrix();
    camera = glm::translate(camera,-position->getValue());
    return camera;
}

glm::mat4 CoffeeCamera::getMatrix() const
{
    if(orthographic)
        return getOrthographic();
    else
        return getProjection();
}

void CoffeeCamera::setFramebufferSizeObject(QSize *fb)
{
    framebufferSize = fb;
}

bool CoffeeCamera::isOrthographic()
{
    return orthographic;
}

void CoffeeCamera::clearFramebufferSizeObject(){
    framebufferSize = NULL;
}
void CoffeeCamera::setOrthographic(bool value)
{
    orthographic = value;
}

