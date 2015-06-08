#include "coffeeworldopts.h"

#include "coffeeomnilight.h"
#include "coffeecamera.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/objects/coffeeskybox.h"
#include "engine/objects/coffeeparticlesystem.h"
#include "engine/physics/bulletphysics.h"

CoffeeWorldOpts::CoffeeWorldOpts(QObject *renderer) : QObject(renderer)
{
    qDebug("Creating world object: thread=%p",this->thread());
    physics = new BulletPhysics(0,glm::vec3(0,-9.81,0));
    //We reserve one such that it won't allocate too many threads in the future.
    QThreadPool::globalInstance()->reserveThread();
    physicsThread = new QThread(this);
    physicsThread->setObjectName("physics-thread");
    physics->setObjectName("[thread-1,bullet-physics]");
    physics->moveToThread(physicsThread);
    connect(this,SIGNAL(tickPhysics(float)),
            physics.data(),SLOT(tickSimulation(float)),
            Qt::QueuedConnection);
    connect(this,SIGNAL(physicsObjectAdded(PhysicsObject*)),
            physics.data(),SLOT(addObject(PhysicsObject*)),
            Qt::QueuedConnection);
    connect(physicsThread,SIGNAL(started()),
            physics.data(),SLOT(run()));
    physicsThread->start();
}

CoffeeWorldOpts::~CoffeeWorldOpts()
{
    physicsThread->exit(0);
    physicsThread->wait();
    QThreadPool::globalInstance()->releaseThread();
}

QPointer<CoffeeCamera> CoffeeWorldOpts::getCamera()
{
    return camera;
}

void CoffeeWorldOpts::setCamera(QPointer<CoffeeCamera> value)
{
    camera = value;
    value->setParent(this);
}

void CoffeeWorldOpts::addLight(QPointer<CoffeeOmniLight> light)
{
    lights.append(light);
    light->setParent(this);
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

void CoffeeWorldOpts::addObject(CoffeeObject *object)
{
    if(object->physics()){
        connect(object->physics(),SIGNAL(deleteObject(void*)),
                physics,SLOT(removeObject(void*)),Qt::QueuedConnection);
        physicsObjectAdded(object->physics());
    }
    objects.append(object);
}

QList<CoffeeObject*> &CoffeeWorldOpts::getObjects()
{
    return objects;
}

void CoffeeWorldOpts::addParticleSystem(CoffeeParticleSystem *system)
{
    particles.append(system);
}

void CoffeeWorldOpts::prepareParticleSystems()
{
    for(CoffeeParticleSystem* s : particles)
        s->setCamera(this->getCamera());
}

QObject *CoffeeWorldOpts::getPhysicsRoot() const
{
    return physics;
}
glm::vec4 CoffeeWorldOpts::getClearColor() const
{
    return clearColor;
}

void CoffeeWorldOpts::setClearColor(const glm::vec4 &value)
{
    clearColor = value;
}

bool CoffeeWorldOpts::wireframeMode() const
{
    return m_wireframeMode;
}

void CoffeeWorldOpts::tickObjects(float d)
{
    for(CoffeeParticleSystem* s : particles)
        s->setFrametime(d);
    tickPhysics(d);
}

void CoffeeWorldOpts::renderWorld()
{
    if(skybox) //rendering the skybox first avoids the color buffer mess with wireframe
        skybox->render();

    if(wireframeMode())
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //will basically take care of skybox, coffeeobject and all the fuzz, but not post-processing.


    for(CoffeeObject* o : this->getObjects()){
        o->render();
    }

    for(CoffeeParticleSystem* s : particles)
        s->render();

    //We need to reset it so that the FBO is rendered
    if(wireframeMode())
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void CoffeeWorldOpts::unloadWorld()
{
    for(CoffeeObject* o : this->getObjects()){
        o->unload();
    }

    for(CoffeeParticleSystem* s : particles)
        s->unload();
}

void CoffeeWorldOpts::setWireframeMode(bool wireframeMode)
{
    m_wireframeMode = wireframeMode;
}
CoffeeSkybox* CoffeeWorldOpts::getSkybox() const
{
    return skybox;
}

void CoffeeWorldOpts::setSkybox(CoffeeSkybox* value)
{
    skybox = value;
}

