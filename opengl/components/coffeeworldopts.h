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
    Q_PROPERTY(glm::vec4 fogColor READ getFogColor WRITE setFogColor)
    Q_PROPERTY(glm::vec4 clearColor READ getClearColor WRITE setClearColor)
    Q_PROPERTY(float fogDensity READ getFogDensity WRITE setFogDensity)

    Q_PROPERTY(QObject* camera READ getCameraQObject WRITE setCameraQObject)
    //An OpenGL-level representation of the world or scene, not meant for modification. That will be done elsewhere.
    Q_OBJECT
public:
    CoffeeWorldOpts(QObject *renderer);
    ~CoffeeWorldOpts();

    QPointer<CoffeeCamera> getCamera();
    QObject* getCameraQObject();

    QList<QPointer<CoffeeOmniLight> > &getLights();
    glm::vec4 getFogColor() const;
    float getFogDensity() const;
    glm::vec4 getClearColor() const;
    bool wireframeMode() const;

    QPointer<CoffeeRenderer> getRenderer();
    void setRenderer(const QPointer<CoffeeRenderer> &value);

    QList<CoffeeObject *> &getObjects();

    void prepareParticleSystems();

    QObject* getPhysicsRoot() const;

    CoffeeSkybox* getSkybox() const;
    void setSkybox(CoffeeSkybox *value);


signals:
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

    void setClearColor(const glm::vec4 &value);
    void setWireframeMode(bool wireframeMode);
    void setFogDensity(float value);
    void setFogColor(const glm::vec4 &value);

    void addLight(QPointer<CoffeeOmniLight> light);
    void setCamera(QPointer<CoffeeCamera> value);
    void setCameraQObject(QObject* camera);

    void addObject(CoffeeObject* object);
    void addParticleSystem(CoffeeParticleSystem* system);


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
