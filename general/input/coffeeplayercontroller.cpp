#include "coffeeplayercontroller.h"

CoffeePlayerController::CoffeePlayerController(QObject *parent) : QObject(parent)
{
    position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    rotation = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
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

void CoffeePlayerController::setRotationPitch(float p)
{
    (*rotation)[0].x = p;
}

void CoffeePlayerController::setRotationYaw(float p)
{
    (*rotation)[0].y = p;
}

void CoffeePlayerController::addRotation(glm::vec3 d)
{
    *rotation+=d;
}

void CoffeePlayerController::addSpeed(glm::vec3 d)
{
    position->setVelocity(d);
}

void CoffeePlayerController::addAccel(glm::vec3 d)
{
    position->setAcceleration(position->getAcceleration()+d);
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
