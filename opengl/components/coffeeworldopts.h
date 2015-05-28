#ifndef COFFEEWORLDOPTS_H
#define COFFEEWORLDOPTS_H
#include "general/common.h"
#include "coffeecamera.h"
#include "coffeeomnilight.h"
#include <QtConcurrent/QtConcurrent>
#include "opengl/context/coffeerenderer.h"
#include "engine/objects/coffeeobject.h"
#include "engine/physics/bulletphysics.h"

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

    void addObject(QPointer<CoffeeObject> object);
    QList<QPointer<CoffeeObject>> &getObjects();

    QObject* getPhysicsRoot() const;

    glm::vec4 getClearColor() const;
    void setClearColor(const glm::vec4 &value);
    bool wireframeMode() const;

signals:
    void tickPhysics(float d);
    void physicsObjectAdded(PhysicsObject* object);
public slots:
    void tickObjects(float d);
    void renderWorld();

    void setWireframeMode(bool wireframeMode);

private:

    QThread* physicsThread;
    QPointer<BulletPhysics> physics;

    float fogDensity = 0.01;
    glm::vec4 fogColor;

    glm::vec4 clearColor;

    QList<QPointer<CoffeeObject>> objects;
    QPointer<CoffeeCamera> camera;
    QList<QPointer<CoffeeOmniLight> > lights;
    QPointer<CoffeeRenderer> renderer;
    bool m_wireframeMode = false;
};

#endif // COFFEEWORLDOPTS_H
