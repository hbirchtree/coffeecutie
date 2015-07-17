#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H
#include "general/common.h"

#include <QColor>

class CoffeeCamera;
class CoffeeSkybox;
class CoffeeRenderer;
class BulletPhysics;
class CoffeeObject;
class CoffeeOmniLight;
class CoffeeParticleSystem;
class PhysicsObject;
class CoffeePlayerController;

class ScalarValue;
class Vector4Value;

class CoffeeWorldOpts : public QObject
{
    Q_PROPERTY(bool loadedState READ loadedState WRITE setLoadedState)
    Q_PROPERTY(bool wireframeMode READ wireframeMode WRITE setWireframeMode)

    Q_PROPERTY(QObject* physicsWorld READ physicsWorld)

    Q_PROPERTY(QObject* fogColor READ getFogColorVariant)
    Q_PROPERTY(QObject* fogDensity READ getFogDensityVariant)

    Q_PROPERTY(QObjectList lights READ getVariantLights)
    Q_PROPERTY(QObject* camera READ getCameraQObject WRITE setCameraQObject)
    Q_PROPERTY(QObject* skybox READ getSkyboxQObject WRITE setSkybox)

    Q_OBJECT
public:
    CoffeeWorldOpts(QObject *parent);
    ~CoffeeWorldOpts();

    QPointer<CoffeeCamera> getCamera();
    QObject* getCameraQObject();

    QList<QPointer<CoffeeOmniLight> > &getLights();
    bool wireframeMode() const;

    QPointer<CoffeeRenderer> getRenderer();

    QList<CoffeeObject *> &getObjects();

    void prepareParticleSystems();

    CoffeeSkybox* getSkybox() const;

    QObjectList getVariantLights();
    QObject* getFogColorVariant();
    QObject* getFogDensityVariant();

    bool loadedState() const;

    QObject* physicsWorld() const;

    QObject* getSkyboxQObject();

signals:
    void physicsObjectAdded(PhysicsObject* object);

public slots:
    void tickObjects(float d);
    void renderWorld();
    void unloadWorld();

    void setWireframeMode(bool wireframeMode);

    void addLight(CoffeeOmniLight* light);
    void setCamera(CoffeeCamera* value);
    void setCameraQObject(QObject* camera);
    void setRendererP(CoffeeRenderer* value);
    void setRenderer(QObject* value);
    void setSkybox(QObject *value);
    void setSkybox(CoffeeSkybox *value);

    void injectPhysicsObject(PhysicsObject* object);
    void addObject(CoffeeObject* object);
    void addParticleSystem(CoffeeParticleSystem* system);

    void connectSignals(QObject* controller);
    void connectSignals(CoffeePlayerController* controller);
    void disconnectSignals();
    //TODO : Disconnect-version of the above, necessary for switching world objects

    void setLoadedState(bool loadedState);

private:
    QThread* physicsThread;
    QPointer<BulletPhysics> physics;

    ScalarValue* fogDensity;
    Vector4Value* fogColor;
    Vector4Value* clearColor;

    QPointer<CoffeeSkybox> skybox;
    QList<CoffeeObject*> objects;
    QPointer<CoffeeCamera> camera;
    QList<QPointer<CoffeeOmniLight> > lights;
    QList<QPointer<CoffeeParticleSystem>> particles;
    QPointer<CoffeeRenderer> renderer;
    bool m_wireframeMode = false;
    bool c_wireframed = false;

    bool m_loadedState = false;

    QVector<QMetaObject::Connection> connections;
};

#endif // COFFEEWORLDOPTS_H
