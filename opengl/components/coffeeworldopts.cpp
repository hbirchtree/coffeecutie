#include "coffeeworldopts.h"

#include "coffeeomnilight.h"
#include "coffeecamera.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/objects/coffeeskybox.h"
#include "engine/objects/coffeeparticlesystem.h"
#include "engine/physics/bulletphysics.h"
#include "general/shadervariant.h"
#include "general/input/coffeeplayercontroller.h"

#include "engine/scripting/qscriptvectorvalue.h"

CoffeeWorldOpts::CoffeeWorldOpts(QObject *parent) : QObject(parent)
{
    qDebug("Creating world object: thread=%p",this->thread());
    physics = new BulletPhysics(0,glm::vec3(0,-9.81,0));
    //We reserve one such that it won't allocate too many threads in the future.
    QThreadPool::globalInstance()->reserveThread();
    physicsThread = new QThread(this);
    physicsThread->setObjectName("physics-thread");
    physics->setObjectName("bullet");
    physics->moveToThread(physicsThread);
    connect(this,SIGNAL(physicsEventReceived(CoffeePhysicsEvent*)),
            physics.data(),SLOT(handlePhysicsEvent(CoffeePhysicsEvent*)),
            Qt::QueuedConnection);
    connect(physicsThread,SIGNAL(started()),
            physics.data(),SLOT(run()));
    physicsThread->start();
    //The physics thread runs on its own tick.

    fogDensity = new ScalarValue(this,0.01f);
    fogColor = new Vector4Value(this,glm::vec4(1,1,1,1));
    clearColor = new Vector4Value(this,glm::vec4(1,1,1,1));
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

QObject *CoffeeWorldOpts::getCameraQObject()
{
    return camera;
}

void CoffeeWorldOpts::setCamera(CoffeeCamera *value)
{
    camera = value;
    value->setParent(this);
}

void CoffeeWorldOpts::setCameraQObject(QObject *camera)
{
    CoffeeCamera* p = qobject_cast<CoffeeCamera*>(camera);
    if(p)
        this->camera = p;
}

void CoffeeWorldOpts::addLight(CoffeeOmniLight* light)
{
    lights.append(light);
    light->setParent(this);
}

QList<QPointer<CoffeeOmniLight> > &CoffeeWorldOpts::getLights()
{
    return lights;
}

QObjectList CoffeeWorldOpts::getVariantLights()
{
    QObjectList l;
    for(QPointer<CoffeeOmniLight> p : lights)
        l.append(p.data());
    return l;
}

QPointer<CoffeeRenderer> CoffeeWorldOpts::getRenderer()
{
    return renderer;
}

void CoffeeWorldOpts::setRendererP(CoffeeRenderer *value)
{
    renderer = value;
    QSize s = renderer->getCurrentFramebufferSize();
    camera->setAspect((float)s.width()/(float)s.height());
}

void CoffeeWorldOpts::setRenderer(QObject *value)
{
    CoffeeRenderer* r = qobject_cast<CoffeeRenderer*>(value);
    if(r)
        setRendererP(r);
    else
        qDebug("Invalid argument!");
}

void CoffeeWorldOpts::setSkybox(QObject *value)
{
    CoffeeSkybox * s = qobject_cast<CoffeeSkybox*>(value);
    if(s)
        setSkybox(s);
}

void CoffeeWorldOpts::addObject(CoffeeObject *object)
{
    objects.append(object);
}

void CoffeeWorldOpts::addObject(QObject *object)
{
    CoffeeObject* o = qobject_cast<CoffeeObject*>(object);
    if(o)
        addObject(o);
}

QList<CoffeeObject*> &CoffeeWorldOpts::getObjects()
{
    return objects;
}

void CoffeeWorldOpts::addParticleSystem(CoffeeParticleSystem *system)
{
    particles.append(system);
}

void CoffeeWorldOpts::connectSignals(QObject *controller)
{
    CoffeePlayerController* c = qobject_cast<CoffeePlayerController*>(controller);
    if(c)
        connectSignals(c);
}

void CoffeeWorldOpts::connectSignals(CoffeePlayerController *controller)
{
    connections.append(connect(controller,&CoffeePlayerController::rotateCamera,[=](glm::vec3 d){
        getCamera()->offsetOrientation(d.y,d.x);
    }));
//    connections.append(connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
//        getCamera()->getPosition()->operator+=(glm::vec3(d));
//    }));
//    connections.append(connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
//        getCamera()->getPosition()->operator+=(glm::vec3(d));
//    }));
}

void CoffeeWorldOpts::disconnectSignals()
{
    for(QMetaObject::Connection c : connections)
        disconnect(c);
    connections.clear();
}

void CoffeeWorldOpts::setLoadedState(bool loadedState)
{
    m_loadedState = loadedState;
}

void CoffeeWorldOpts::prepareParticleSystems()
{
    for(CoffeeParticleSystem* s : particles)
        s->setCamera(this->getCamera());
}

bool CoffeeWorldOpts::wireframeMode() const
{
    return m_wireframeMode;
}

void CoffeeWorldOpts::tickObjects(float d)
{
    for(CoffeeParticleSystem* s : particles)
        s->setFrametime(d);
}

void CoffeeWorldOpts::renderWorld()
{
    if(!loadedState()){
        qDebug() << "Setting OpenGL clear-color:" << clearColor->asColor();
        glm::vec4 cc = clearColor->getValue();
        renderer->setClearColor(cc.r, cc.g, cc.b, cc.a);
        setLoadedState(true);
    }

    if(skybox) //rendering the skybox first avoids the color buffer mess with wireframe
        skybox->render();

    if(wireframeMode()){
        c_wireframed = true;
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }

    for(CoffeeObject* o : objects)
        o->render();

    for(CoffeeParticleSystem* s : particles)
        s->render();

    //We need to reset it so that the FBO is rendered
    if(c_wireframed){
        c_wireframed = false;
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
}

void CoffeeWorldOpts::unloadWorld()
{
    //We queue it up for the render-thread to execute it. (Otherwise, it may run on the script thread or elsewhere (!))
    std::function<void()> fp = [=](){
        for(CoffeeObject* o : this->getObjects()){
            o->unload();
        }

        for(CoffeeParticleSystem* s : particles)
            s->unload();
    };
    renderer->queueFunction(fp);
    setLoadedState(false);
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

QObject *CoffeeWorldOpts::getFogColorVariant()
{
    return fogColor;
}

QObject *CoffeeWorldOpts::getFogDensityVariant()
{
    return fogDensity;
}

bool CoffeeWorldOpts::loadedState() const
{
    return m_loadedState;
}

QObject *CoffeeWorldOpts::physicsWorld() const
{
    return physics;
}

QObject *CoffeeWorldOpts::getSkyboxQObject()
{
    return skybox;
}

QObject *CoffeeWorldOpts::clearColorValue() const
{
    return clearColor;
}

void CoffeeWorldOpts::handlePhysEvent(CoffeePhysicsEvent *ev)
{
    physicsEventReceived(ev);
}
