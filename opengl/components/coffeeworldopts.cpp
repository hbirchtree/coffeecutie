#include "coffeeworldopts.h"

#include "coffeeomnilight.h"
#include "coffeecamera.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/objects/coffeeskybox.h"
#include "engine/objects/coffeeparticlesystem.h"
#include "engine/physics/bulletphysics.h"
#include "general/shadervariant.h"
#include "general/input/coffeeplayercontroller.h"

CoffeeWorldOpts::CoffeeWorldOpts(QObject *renderer) : QObject(renderer)
{
    qDebug("Creating world object: thread=%p",this->thread());
    physics = new BulletPhysics(0,glm::vec3(0,-9.81,0));
    //We reserve one such that it won't allocate too many threads in the future.
    QThreadPool::globalInstance()->reserveThread();
    physicsThread = new QThread(this);
    physicsThread->setObjectName("physics-thread");
    physics->setObjectName("bullet");
    physics->moveToThread(physicsThread);
//    connect(this,SIGNAL(tickPhysics(float)),
//            physics.data(),SLOT(tickSimulation(float)),
//            Qt::QueuedConnection);
    connect(this,SIGNAL(physicsObjectAdded(PhysicsObject*)),
            physics.data(),SLOT(addObject(PhysicsObject*)),
            Qt::QueuedConnection);
    connect(physicsThread,SIGNAL(started()),
            physics.data(),SLOT(run()));
    physicsThread->start();

    this->fogColorVariant = new ShaderVariant([=](){
        return this->getFogColor();
    });
    this->fogDensityVariant = new ShaderVariant([=](){
        return this->getFogDensity();
    });
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

void CoffeeWorldOpts::setCamera(QPointer<CoffeeCamera> value)
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

void CoffeeWorldOpts::injectPhysicsObject(PhysicsObject *object)
{
    physicsObjectAdded(object); //kind of a hack on our current ways, but it will work.
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

QObjectList CoffeeWorldOpts::getVariantLights()
{
    QObjectList l;
    for(QPointer<CoffeeOmniLight> p : lights)
        l.append(p.data());
    return l;
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

void CoffeeWorldOpts::setRendererP(QPointer<CoffeeRenderer> value)
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

void CoffeeWorldOpts::setFogColorValue(QColor fogColor)
{
    qDebug() << "New color:" << fogColor;
}

void CoffeeWorldOpts::setClearColorValue(QColor clearColor)
{
    qDebug() << "New color:" << clearColor;
}

void CoffeeWorldOpts::connectSignals(CoffeePlayerController *controller)
{
    connect(renderer.data(),&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        getCamera()->setAspect((float)ev.size().width()/(float)ev.size().height());
    });
    connect(controller,&CoffeePlayerController::rotateCamera,[=](glm::vec3 d){
        getCamera()->offsetOrientation(d.y,d.x);
    });
    connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
        getCamera()->getPosition()->operator+=(glm::vec3(d));
    });
    connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
        getCamera()->getPosition()->operator+=(glm::vec3(d));
    });
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
}

void CoffeeWorldOpts::renderWorld()
{
    if(!loadedState()){
        qDebug() << "Setting OpenGL clear-color:" << clearColorValue();
        renderer->updateRendererClearColor(getClearColor());
        setLoadedState(true);
    }

    if(skybox) //rendering the skybox first avoids the color buffer mess with wireframe
        skybox->render();

    if(wireframeMode()){
        c_wireframed = true;
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }

    //will basically take care of skybox, coffeeobject and all the fuzz, but not post-processing.
    for(CoffeeObject* o : this->getObjects())
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

QColor CoffeeWorldOpts::fogColorValue() const
{
    return QColor(qRgba(fogColor.x*255,fogColor.y*255,fogColor.z*255,fogColor.w*255));
}

QColor CoffeeWorldOpts::clearColorValue() const
{
    return QColor(qRgba(clearColor.x*255,clearColor.y*255,clearColor.z*255,clearColor.w*255));
}

QObject *CoffeeWorldOpts::getFogColorVariant()
{
    return fogColorVariant;
}

QObject *CoffeeWorldOpts::getFogDensityVariant()
{
    return fogDensityVariant;
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
