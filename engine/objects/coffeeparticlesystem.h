#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include "engine/objects/coffeeobject.h"
#include <QColor>

class CoffeeShader;
class CoffeeTexture;
class CoffeeCamera;
class CoffeeTransformComputer;

class CoffeeParticleSystem : public QObject,public CoffeeObject
{
    Q_PROPERTY(bool additive READ additive WRITE setAdditive)

    Q_PROPERTY(QObject* camera WRITE setCamera)
    Q_PROPERTY(QObject* texture READ getTextureQObject WRITE setTexture)
    Q_PROPERTY(QObject* transform READ getTransformObject)

    Q_PROPERTY(QObject* particleColor READ particleColor)
    Q_PROPERTY(QObject* gravity READ gravityQObject)

    Q_PROPERTY(QObject* particleSize READ particleSize)
    Q_PROPERTY(QObject* particleMass READ particleMass)
    Q_PROPERTY(QObject* particleSpread READ particleSpread)

    Q_OBJECT

public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera = nullptr);

    bool isBaked();
    void setBaked(bool val);

    void setCamera(const CoffeeCamera *value);

    QPointer<CoffeeTexture> getTexture();

    QPointer<CoffeeShader> getShader();
    void setShader(QPointer<CoffeeShader> value);

    CoffeeTransformComputer* getTransform();

    bool additive() const;

    QObject* getTransformObject();
    QObject* getTextureQObject();
    QObject* gravityQObject();

    QObject* particleColor();
    QObject* particleSize();
    QObject* particleMass();
    QObject* particleSpread();

signals:
    void requestTick(float d);

public slots:
    void render();
    void unload();
    void load();

    void setFrametime(float time);
    void setAdditive(bool additive);
    void setTexture(CoffeeTexture* value);
    void setTexture(QObject* value);

    void setGravity(Vector3Value* gravity);
    void setGravity(QObject* gravity);

    void setCamera(const QObject *value);

protected:
    CoffeeTransformComputer* transform = nullptr;
    void renderParticles();

    bool baked = false;
    CoffeeShader* shader = nullptr;
    CoffeeTexture* texture = nullptr;
    const CoffeeCamera* m_camera = nullptr;

private:
    bool m_additive = false;
    bool c_additive; //so that we won't miss on swapping blendfunction

    ScalarValue* m_particleMass;
    ScalarValue* m_particleSpread;
    ScalarValue* m_particleSize;
    Vector3Value* m_gravity;
    Vector4Value* m_particleColor;

    ScalarValue* m_spawnCount;
    ScalarValue* m_frametime;
    ScalarValue* m_randomDir;
    ScalarValue* m_randomAmplitude;
};

#endif // COFFEEPARTICLESYSTEM_H
