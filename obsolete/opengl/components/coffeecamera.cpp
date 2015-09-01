#include "coffeecamera.h"

#include <math.h>

#include "engine/scripting/qscriptvectorvalue.h"

CoffeeCamera::CoffeeCamera(QObject *parent) : QObject(parent)
{
    m_rotationLimit.setX(-70.f);
    m_rotationLimit.setY(90.f);

    this->aspect = new ScalarValue(this,1.f);
    aspect->setObjectName("aspect_ratio");
    this->fov = new ScalarValue(this,90.f);
    fov->setObjectName("fov");
    this->position = new Vector3Value(this,glm::vec3(0,0,0));
    position->setObjectName("position");
    this->orientation = new QuatValue(this,glm::quat(1,0,0,0));
    orientation->setObjectName("quat_orientation");
    this->rotation_euler = new Vector3Value(this,glm::vec3(0,0,0));
    rotation_euler->setObjectName("euler_rotation");

    this->matrixVal = new Matrix4Value(this,[=](const glm::mat4& v){
        Q_UNUSED(v)
        return getMatrix();
    });
    matrixVal->setObjectName("default");
    this->cameraForwardVal = new Vector3Value(this,[=](const glm::vec3& v){
        Q_UNUSED(v)
        return getCameraForwardNormal();
    });
    cameraForwardVal->setObjectName("forward");
    this->cameraRightVal = new Vector3Value(this,[=](const glm::vec3& v){
        Q_UNUSED(v)
        return getCameraRightNormal();
    });
    cameraRightVal->setObjectName("right");
    this->matrixVPVal = new Matrix4Value(this,[=](const glm::mat4& v){
        Q_UNUSED(v)
        return getPerspective();
    });
    matrixVPVal->setObjectName("viewproject");
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
    this->rotation_euler->setValue(rot);
}

CoffeeCamera::~CoffeeCamera()
{
}

QPointer<Vector3Value> CoffeeCamera::getPosition()
{
    return position;
}

QPointer<Vector3Value> CoffeeCamera::getRotation()
{
    return rotation_euler;
}

QPointer<ScalarValue> CoffeeCamera::getFieldOfView()
{
    return fov;
}

QPointer<ScalarValue> CoffeeCamera::getAspect()
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
                glm::radians(upAngle),
                glm::radians(rightAngle),
                0);
    normalizeEulerAngles(rotation_euler,glm::radians(m_rotationLimit.x()),
                         glm::radians(m_rotationLimit.y()));
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

glm::vec3 CoffeeCamera::getCameraPos() const
{
    return position->getValue();
}
float CoffeeCamera::getCameraForwardDirection() const{
    return getCameraForwardNormal().y;
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
    return ori;
}

glm::mat4 CoffeeCamera::getProjection() const
{
    glm::mat4 camera = getPerspective();
    camera *= getOrientationMatrix();
    return glm::translate(camera,-position->getValue());
}

glm::mat4 CoffeeCamera::getOrthographic() const
{
    glm::mat4 camera = glm::ortho(0.f,16.f,0.f,10.f,znear,zfar);
    camera *= getOrientationMatrix();
    return glm::translate(camera,-position->getValue());
}

glm::mat4 CoffeeCamera::getPerspective() const
{
    return glm::perspective(glm::radians(fov->getValue()),
                            aspect->getValue(),
                            znear,zfar);
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

void CoffeeCamera::normalizeEulerAngles(QPointer<Vector3Value> e, float x_min, float x_max)
{
    glm::vec3 v = e->getValue();

    if(std::abs(v.y)>M_PI*2.f)
        v.y = std::fmod(v.y,M_PI*2.f);
    if(v.y<0.f)
        v.y += M_PI*2.f;

    if(v.x<x_min)
        v.x = x_min;
    if(v.x>x_max)
        v.x = x_max;

    *e=v;
}

bool CoffeeCamera::isOrthographic() const
{
    return orthographic;
}

void CoffeeCamera::clearFramebufferSizeObject(){
    framebufferSize = nullptr;
}

void CoffeeCamera::setFov(float fov)
{
    this->fov->operator =(fov);
}
float CoffeeCamera::getZfar() const
{
    return zfar;
}

void CoffeeCamera::setZfar(float value)
{
    zfar = value;
}

float CoffeeCamera::getAspectValue() const
{
    return aspect->getValue();
}

void CoffeeCamera::setAspect(float value)
{
    aspect->setValue(value);
}

float CoffeeCamera::getFov() const
{
    return fov->getValue();
}

QObject *CoffeeCamera::getMatrixVPVariant() const
{
    return matrixVPVal;
}

QObject *CoffeeCamera::getMatrixVariant() const
{
    return matrixVal;
}

QObject *CoffeeCamera::getPositionValue()
{
    return position;
}

QObject *CoffeeCamera::getRotationValue()
{
    return rotation_euler;
}

QObject *CoffeeCamera::getCameraRightVariant() const
{
    return cameraRightVal;
}

QObject *CoffeeCamera::getCameraForwardVariant() const
{
    return cameraForwardVal;
}

float CoffeeCamera::getZnear() const
{
    return znear;
}

void CoffeeCamera::setZnear(float value)
{
    znear = value;
}

void CoffeeCamera::setOrthographic(bool value)
{
    orthographic = value;
}

