#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H
#include "general/common.h"
#include "engine/physics/genericphysicsinterface.h"

class CoffeeCamera;
class CoffeeSkybox;
class CoffeeRenderer;
class BulletPhysics;
class CoffeeObject;
class CoffeeOmniLight;
class CoffeeParticleSystem;
class PhysicsObject;

class CoffeeWorldOpts : public QObject
{
    Q_PROPERTY(bool wireframeMode READ wireframeMode WRITE setWireframeMode)
    //An OpenGL-level representation of the world or scene, not meant for modification. That will be done elsewhere.
    Q_OBJECT
public:
    CoffeeWorldOpts(QObject *renderer);
    ~CoffeeWorldOpts();

    QPointer<CoffeeCamera> getCamera();
    void setCamera(QPointer<CoffeeCamera> value);

    void addLight(QPointer<CoffeeOmniLight> light);
    QList<QPointer<CoffeeOmniLight> > &getLights();

    glm::vec4 getFogColor() const;
    void setFogColor(const glm::vec4 &value);

    float getFogDensity() const;
    void setFogDensity(float value);

    QPointer<CoffeeRenderer> getRenderer();
    void setRenderer(const QPointer<CoffeeRenderer> &value);

    void addObject(CoffeeObject* object);
    QList<CoffeeObject *> &getObjects();

    void addParticleSystem(CoffeeParticleSystem* system);
    void prepareParticleSystems();

    QObject* getPhysicsRoot() const;

    glm::vec4 getClearColor() const;
    void setClearColor(const glm::vec4 &value);
    bool wireframeMode() const;

    CoffeeSkybox* getSkybox() const;
    void setSkybox(CoffeeSkybox *value);

signals:
    void tickPhysics(float d);
    void modifyPhysics(PhysicsObject *object,
                       GenericPhysicsInterface::PhysicsProperty prop,
                       const VectorVariant &value);
    void physicsObjectAdded(PhysicsObject* object);
public slots:
    void physicsModify(PhysicsObject *object,
                       GenericPhysicsInterface::PhysicsProperty prop,
                       const VectorVariant &value);
    void tickObjects(float d);
    void renderWorld();
    void unloadWorld();

    void setWireframeMode(bool wireframeMode);

private:

    QThread* physicsThread;
    QPointer<BulletPhysics> physics;

    float fogDensity = 0.01;
    glm::vec4 fogColor;

    glm::vec4 clearColor;

    QPointer<CoffeeSkybox> skybox;
    QList<CoffeeObject*> objects;
    QPointer<CoffeeCamera> camera;
    QList<QPointer<CoffeeOmniLight> > lights;
    QList<QPointer<CoffeeParticleSystem>> particles;
    QPointer<CoffeeRenderer> renderer;
    bool m_wireframeMode = false;
};

#endif // COFFEEWORLDOPTS_H
