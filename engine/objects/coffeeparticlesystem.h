#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include "engine/objects/coffeeobject.h"
#include <QColor>

class ShaderContainer;
class ShaderVariant;
class CoffeeTexture;
class CoffeeCamera;
class CoffeeTransformComputer;

class CoffeeParticleSystem : public QObject,public CoffeeObject
{
    Q_PROPERTY(bool additive READ additive WRITE setAdditive)
    Q_PROPERTY(QColor particleColor READ getParticleColor WRITE setParticleColor)
    Q_PROPERTY(float particleSize READ getParticleSize WRITE setParticleSize)

    Q_PROPERTY(QObject* transform READ getTransformObject)

    Q_OBJECT

public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    bool isBaked();
    void setBaked(bool val);

    float getParticleSize() const;
    QColor getParticleColor() const;

    void setCamera(const CoffeeCamera *value);

    QPointer<CoffeeTexture> getTexture();
    void setTexture(QPointer<CoffeeTexture> value);

    QPointer<ShaderContainer> getShader();
    void setShader(QPointer<ShaderContainer> value);

    CoffeeTransformComputer* getTransform();

    bool additive() const;

    QObject* getTransformObject();

signals:
    void requestTick(float d);

public slots:
    void render();
    void unload();
    void load();

    void setFrametime(float time);
    void setParticleSize(float particleSize);
    void setParticleColor(QColor particleColor);
    void setAdditive(bool additive);

protected:
    CoffeeTransformComputer* transform = nullptr;
    void renderParticles();

    glm::vec4 particleColor;

    bool baked = false;
    QPointer<ShaderContainer> shader;
    QPointer<CoffeeTexture> texture;
    const CoffeeCamera* camera;

    float particleSize = 0.01f;

    float tickTime = 0.001f;
private:
    bool m_additive = false;
    bool c_additive; //so that we won't miss on swapping blendfunction
    QObject* m_transform;

    ShaderVariant* frametime;
    ShaderVariant* mass;
    ShaderVariant* gravity;
    ShaderVariant* randRad;
    ShaderVariant* randAmp;
    ShaderVariant* partSpread;
    ShaderVariant* spawncount;
};

#endif // COFFEEPARTICLESYSTEM_H
