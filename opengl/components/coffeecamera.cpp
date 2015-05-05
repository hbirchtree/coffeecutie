#include "coffeecamera.h"

CoffeeCamera::CoffeeCamera(QObject *parent) : QObject(parent)
{
    this->aspect = new NumberContainer<float>(this,1.f);
    this->fov = new NumberContainer<float>(this,90.f);
    this->position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    this->rotation = new NumberContainer<glm::quat>(this,glm::quat(1,0,0,0));
    this->rotation_euler = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
//    rotation->setClamps(glm::vec3(-90,0,0),glm::vec3(90,0,0));
}

CoffeeCamera::CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov) : CoffeeCamera(parent)
{
    this->aspect->setValue(aspect);
    this->fov->setValue(fov);
    this->znear = znear;
    this->zfar = zfar;
}

CoffeeCamera::CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::quat rot) : CoffeeCamera(parent,aspect,znear,zfar,fov)
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

QPointer<NumberContainer<glm::vec3>> CoffeeCamera::getPosition()
{
    return position;
}

QPointer<NumberContainer<glm::quat>> CoffeeCamera::getRotation()
{
    return rotation;
}

QPointer<NumberContainer<float>> CoffeeCamera::getFieldOfView()
{
    return fov;
}

QPointer<NumberContainer<float>> CoffeeCamera::getAspect()
{
    return aspect;
}

void CoffeeCamera::offsetOrientation(float rightAngle, float upAngle)
{
//    rotation->setValue(glm::normalize(rotation->getValue()));
//    glm::vec3 curr = glm::eulerAngles(rotation->getValue());
//    curr.y+=rightAngle*QuickMath::Math_DegToRadFactor();
//    curr.x+=upAngle*QuickMath::Math_DegToRadFactor();
    *rotation_euler += glm::vec3(
                upAngle*QuickMath::Math_DegToRadFactor(),
                rightAngle*QuickMath::Math_DegToRadFactor(),
                0);
    normalizeEulerAngles(rotation_euler,glm::radians(-70.f),glm::radians(90.f));
//    glm::quat rot(0.f,rotation_euler->getValue());
//    *rotation=rot*rotation->getValue();
}

void CoffeeCamera::cameraLookAt(glm::vec3 point)
{
    glm::lookAt(position->getValue(),point,glm::vec3(0,1,0));
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
    glm::mat4 ori = glm::rotate(glm::mat4(),
                      rotation_euler->getValue().x,
                      glm::vec3(1,0,0));
    ori = glm::rotate(ori,
                      rotation_euler->getValue().y,
                      glm::vec3(0,1,0));
    ori = glm::rotate(ori,
                      rotation_euler->getValue().z,
                      glm::vec3(0,0,1));
//    qDebug() << QStringFunctions::toString(ori);
    return ori;
//    return glm::mat4(rotation->getValue());
}

glm::mat4 CoffeeCamera::getProjection() const
{
    glm::mat4 camera = glm::perspective(QuickMath::math_degreesToRads(fov->getValue()),aspect->getValue(),znear,zfar);
//    camera *= glm::lookAt(position->getValue(),glm::vec3(0,0,0),glm::vec3(0,1,0));
    camera *= getOrientationMatrix();
    camera = glm::translate(camera,-position->getValue());
    return camera;
}

glm::mat4 CoffeeCamera::getOrthographic() const
{
    QSize framebufferSize(800,600);
//    if(framebufferSize==NULL)
//        framebufferSize
    glm::mat4 camera = glm::ortho(0,framebufferSize.width(),framebufferSize.height(),0);
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

void CoffeeCamera::normalizeEulerAngles(QPointer<NumberContainer<glm::vec3>> e, float x_min, float x_max)
{
    glm::vec3 v = e->getValue();

    if(std::abs(v.y)>QuickMath::Math_Pi*2.f)
        v.y = std::fmod(v.y,QuickMath::Math_Pi*2.f);
    if(v.y<0.f)
        v.y += QuickMath::Math_Pi*2.f;

    if(v.x<x_min)
        v.x = x_min;
    if(v.x>x_max)
        v.x = x_max;

    *e=v;
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

