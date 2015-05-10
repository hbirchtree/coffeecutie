#include "coffeeworldopts.h"

CoffeeWorldOpts::CoffeeWorldOpts(QObject *renderer) : QObject(renderer)
{
    qDebug("Creating world object: thread=%p",this->thread());
    fogColor.g=1;
    physicsThread = new QThread(this);
    physics = new BulletPhysics(0,glm::vec3(0,-9.81,0));
    connect(this,SIGNAL(tickPhysics(float)),
            physics.data(),SLOT(tickSimulation(float)),
            Qt::QueuedConnection);
    connect(this,SIGNAL(physicsObjectAdded(PhysicsObject*)),
            physics.data(),SLOT(addObject(PhysicsObject*)),
            Qt::QueuedConnection);
    connect(this,SIGNAL(physicsClose()),
            physics.data(),SLOT(stopThread()),
            Qt::QueuedConnection);
    physics->moveToThread(physicsThread);
    connect(physicsThread,SIGNAL(started()),
            physics.data(),SLOT(run()),
            Qt::QueuedConnection);
    connect(physicsThread,SIGNAL(finished()),
            physics.data(),SLOT(deleteLater()));
    physicsThread->start();
}

CoffeeWorldOpts::~CoffeeWorldOpts()
{
    physicsClose();
    physicsThread->exit(0);
    physicsThread->wait();
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

QList<QPointer<CoffeeOmniLight> > &CoffeeWorldOpts::getLights()
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

void CoffeeWorldOpts::addObject(QPointer<CoffeeObject> object)
{
    if(object->getPhysicsObject()){
        connect(object->getPhysicsObject(),SIGNAL(deleteObject(void*)),
                physics,SLOT(removeObject(void*)),Qt::QueuedConnection);
        physicsObjectAdded(object->getPhysicsObject());
    }
    objects.append(object);
}

QList<QPointer<CoffeeObject> > &CoffeeWorldOpts::getObjects()
{
    return objects;
}
glm::vec4 CoffeeWorldOpts::getClearColor() const
{
    return clearColor;
}

void CoffeeWorldOpts::setClearColor(const glm::vec4 &value)
{
    clearColor = value;
}

void CoffeeWorldOpts::tickObjects(float d)
{
    tickPhysics(d);
}
