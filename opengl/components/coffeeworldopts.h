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

class CoffeeWorldOpts : public QObject
{
    Q_PROPERTY(bool wireframeMode READ wireframeMode WRITE setWireframeMode)
    Q_PROPERTY(QColor clearColor READ clearColorValue WRITE setClearColorValue)
    Q_PROPERTY(QColor fogColor READ fogColorValue WRITE setFogColorValue)
    Q_PROPERTY(float fogDensity READ getFogDensity WRITE setFogDensity)

    Q_PROPERTY(QObject* camera READ getCameraQObject WRITE setCameraQObject)

    Q_PROPERTY(QObject* fogColorVariant READ getFogColorVariant)
    Q_PROPERTY(QObject* fogDensityVariant READ getFogDensityVariant)

    Q_PROPERTY(QObjectList lights READ getVariantLights)

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

    QColor fogColorValue() const;
    QColor clearColorValue() const;

    QObjectList getVariantLights();
    QObject* getFogColorVariant();
    QObject* getFogDensityVariant();

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

    void injectPhysicsObject(PhysicsObject* object);
    void addObject(CoffeeObject* object);
    void addParticleSystem(CoffeeParticleSystem* system);

    void setFogColorValue(QColor fogColor);
    void setClearColorValue(QColor clearColor);

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

    ShaderVariant* fogColorVariant;
    ShaderVariant* fogDensityVariant;
};

#endif // COFFEEWORLDOPTS_H
