#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H
#include "general/common.h"
#include "engine/physics/genericphysicsinterface.h"

#include <QColor>

class CoffeeCamera;
class CoffeeSkybox;
class CoffeeRenderer;
class BulletPhysics;
class CoffeeObject;
class CoffeeOmniLight;
class CoffeeParticleSystem;
class PhysicsObject;
class ShaderVariant;
class CoffeePlayerController;

class CoffeeWorldOpts : public QObject
{
    Q_PROPERTY(bool loadedState READ loadedState WRITE setLoadedState)
    Q_PROPERTY(bool wireframeMode READ wireframeMode WRITE setWireframeMode)
    Q_PROPERTY(QColor clearColor READ clearColorValue WRITE setClearColorValue)
    Q_PROPERTY(QColor fogColor READ fogColorValue WRITE setFogColorValue)
    Q_PROPERTY(float fogDensity READ getFogDensity WRITE setFogDensity)

    Q_PROPERTY(QObject* physicsWorld READ physicsWorld)

    Q_PROPERTY(QObject* fogColorVariant READ getFogColorVariant)
    Q_PROPERTY(QObject* fogDensityVariant READ getFogDensityVariant)

    Q_PROPERTY(QObjectList lights READ getVariantLights)
    Q_PROPERTY(QObject* camera READ getCameraQObject WRITE setCameraQObject)
    Q_PROPERTY(QObject* skybox READ getSkyboxQObject WRITE setSkybox)

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

    QList<CoffeeObject *> &getObjects();

    void prepareParticleSystems();

    CoffeeSkybox* getSkybox() const;

    QColor fogColorValue() const;
    QColor clearColorValue() const;

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

    void setClearColor(const glm::vec4 &value);
    void setWireframeMode(bool wireframeMode);
    void setFogDensity(float value);
    void setFogColor(const glm::vec4 &value);

    void addLight(QPointer<CoffeeOmniLight> light);
    void setCamera(QPointer<CoffeeCamera> value);
    void setCameraQObject(QObject* camera);
    void setRenderer(const QPointer<CoffeeRenderer> &value);
    void setSkybox(QObject *value);
    void setSkybox(CoffeeSkybox *value);

    void injectPhysicsObject(PhysicsObject* object);
    void addObject(CoffeeObject* object);
    void addParticleSystem(CoffeeParticleSystem* system);

    void setFogColorValue(QColor fogColor);
    void setClearColorValue(QColor clearColor);

    void connectSignals(CoffeePlayerController* controller);

    void setLoadedState(bool loadedState);

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
    bool c_wireframed = false;

    ShaderVariant* fogColorVariant;
    ShaderVariant* fogDensityVariant;
    bool m_loadedState;
};

#endif // COFFEEWORLDOPTS_H
