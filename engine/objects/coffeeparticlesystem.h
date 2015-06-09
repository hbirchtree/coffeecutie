#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include "engine/objects/coffeeobject.h"
#include <QColor>

class ShaderContainer;
class CoffeeTexture;
class CoffeeCamera;

class CoffeeParticleSystem : public QObject,public CoffeeObject
{
    Q_PROPERTY(QColor particleColor READ getParticleColor WRITE setParticleColor)
    Q_PROPERTY(float particleSize READ getParticleSize WRITE setParticleSize)
    Q_PROPERTY(quint32 max_particles READ getMaxParticles WRITE setMaxParticles)
    Q_PROPERTY(float particleMass READ particleMass WRITE setParticleMass)
    Q_PROPERTY(glm::vec3 gravity READ gravity WRITE setGravity)

    Q_OBJECT

public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);


    bool isBaked();
    void setBaked(bool val);

    quint32 getMaxParticles() const;
    float getParticleSize() const;
    QColor getParticleColor() const;
    quint64 getParticleCount() const;
    quint64 getProcessTime() const;
    glm::vec3 gravity() const;
    float particleMass() const;

    void setCamera(const CoffeeCamera *value);

    QPointer<CoffeeTexture> getTexture();
    void setTexture(QPointer<CoffeeTexture> value);

    QPointer<ShaderContainer> getShader();
    void setShader(QPointer<ShaderContainer> value);

    QPointer<ShaderContainer> getTransformShader();
    void setTransformShader(QPointer<ShaderContainer> value);

public slots:
    void render();
    void unload();
    void load();

    void setFrametime(float time);

    void setMaxParticles(quint32 max_particles);
    void setParticleSize(float particleSize);
    void setParticleColor(QColor particleColor);
    void setGravity(const glm::vec3 &gravity);
    void setParticleMass(float particleMass);

protected:
    glm::vec4 particleColor;

    bool baked = false;
    QPointer<ShaderContainer> shader;
    QPointer<ShaderContainer> tshader;
    QPointer<CoffeeTexture> texture;
    const CoffeeCamera* camera;

    struct Particle {
        float type;
        glm::vec3 pos;
        glm::vec3 vel;
        float lifetime;
    };

    bool started = false;

    float frametime = 0.005f;

    uint vaoIndex = 0;
    uint vboIndex = 1;

    GLuint vaos[2];
    GLuint vbos[2];

    uint tfbIndex = 0;
    GLuint tfbs[2];

    GLuint timeQuery;
    GLuint primitiveQuery;
    GLuint queries[2];

    float particleSize = 0.01f;

    QVector<Particle> startParticles;

    quint32 active_particles = 1;
    quint32 max_particles = 10240;

    quint32 spawncount = 20;

    GLuint64 processtime = 0;
private:
    glm::vec3 m_gravity;
    float m_particleMass;
};

#endif // COFFEEPARTICLESYSTEM_H
