#include "coffeeplayercontroller.h"

#include "general/shadervariant.h"

CoffeePlayerController::CoffeePlayerController(QObject *parent) : QObject(parent)
{
    position = new Vector3Value(this,glm::vec3(0,0,0));
    rotation = new Vector3Value(this,glm::vec3(0,0,0));
}

CoffeePlayerController::~CoffeePlayerController()
{
    delete position;
    delete rotation;
}

void CoffeePlayerController::setRotation(glm::vec3 r)
{
    *rotation = r;
}

void CoffeePlayerController::joystickSetRotationPitch(float p)
{
    (*rotation)[0].x = p*joystickSensitivity;
}

void CoffeePlayerController::joystickSetRotationYaw(float p)
{
    (*rotation)[0].y = p*joystickSensitivity;
}

void CoffeePlayerController::mouseSetRotationPointF(const QPointF &offset)
{
    rotateCamera(glm::vec3(offset.y(),offset.x(),0)*mouseSensitivity);
}

void CoffeePlayerController::mouseSetRotation(float pitch, float yaw)
{
    rotateCamera(glm::vec3(yaw,pitch,0)*mouseSensitivity);
}

void CoffeePlayerController::addRotation(glm::vec3 d)
{
    *rotation+=d;
}

void CoffeePlayerController::addSpeedForward(glm::vec3 d)
{
    glm::vec3 velo = position->getVelocity()+d-forwardMove;
    forwardMove = d;
    position->setVelocity(velo);
}

void CoffeePlayerController::addSpeedRight(glm::vec3 d)
{
    glm::vec3 velo = position->getVelocity()+d-rightMove;
    rightMove = d;
    position->setVelocity(velo);
}

void CoffeePlayerController::addAccel(glm::vec3 d)
{
    position->setAcceleration(position->getAcceleration()+d);
}

void CoffeePlayerController::addSpeedForward(Vector3Value *d)
{
    addSpeedForward(d->getValue());
}

void CoffeePlayerController::addSpeedRight(Vector3Value *d)
{
    addSpeedRight(d->getValue());
}

void CoffeePlayerController::addSpeedForward(QObject *d, const QVariantList &factor)
{
    ShaderVariant* v = dynamic_cast<ShaderVariant*>(d);
    if(factor.size()!=3||!v||!v->getVec3())
        return;
    glm::vec3 t_fac = glm::vec3(factor.at(0).toFloat(),factor.at(1).toFloat(),factor.at(2).toFloat());
    addSpeedForward((*v->getVec3())()*t_fac);
}

void CoffeePlayerController::addSpeedRight(QObject *d, const QVariantList &factor)
{
    ShaderVariant* v = dynamic_cast<ShaderVariant*>(d);
    if(factor.size()!=3||!v||!v->getVec3())
        return;
    glm::vec3 t_fac = glm::vec3(factor.at(0).toFloat(),factor.at(1).toFloat(),factor.at(2).toFloat());
    addSpeedRight((*v->getVec3())()*t_fac);
}

void CoffeePlayerController::tick(float time)
{
    position->setVelocity(position->getVelocity()+position->getAcceleration()*time);
    //Limit the speed possible
    if(position->getVelocity().length()>5){
        float rf = 5/position->getVelocity().length();
        position->setVelocity(position->getVelocity()*rf);
    }
    movePlayer(glm::vec4(position->getVelocity()*time,0.f));
    rotateCamera(rotation->getValue()*time);
}
