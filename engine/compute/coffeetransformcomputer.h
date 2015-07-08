#ifndef COFFEETRANSFORMCOMPUTER_H
#define COFFEETRANSFORMCOMPUTER_H

#include "general/common.h"

class ShaderContainer;
class Vector3Value;

class CoffeeTransformComputer : public QObject
{
    Q_PROPERTY(float particleSpread READ particleSpread WRITE setParticleSpread)
    Q_PROPERTY(float particleMass READ particleMass WRITE setParticleMass)
    Q_PROPERTY(quint32 maxParticles READ maxParticles WRITE setMaxParticles)
    Q_PROPERTY(QObject* gravity READ getGravityObject)

    Q_OBJECT
public:
    struct Particle {
        float type;
        glm::vec3 pos;
        glm::vec3 vel;
        float lifetime;
    };

    CoffeeTransformComputer(QObject *parent);

    QObject* getGravityObject();
    glm::vec3 gravity() const;

    float particleSpread() const;
    float particleMass() const;
    quint32 maxParticles() const;

    GLuint getRenderTransform();
    GLuint getRenderArray();

    QVector<Particle>* getParticles();


public slots:
    void tickParticles(float frametime);

    void setParticleSpread(float particleSpread);
    void setParticleMass(float particleMass);
    void setMaxParticles(quint32 maxParticles);

private slots:
    void switchIndex();

private:
    void load();
    void unload();

    uint vboIndex();
    uint vaoIndex();

    bool m_loaded = false;
    bool m_started = false;


    QPointer<ShaderContainer> tshader;

    GLuint tfbs[2];
    GLuint vbos[2];
    GLuint vaos[2];

    GLuint timeQuery;
    GLuint primitiveQuery;

    uint index = 0;
    uint spawncount = 1;
    uint active_particles = 1;

    QVector<Particle> startParticles;
    Vector3Value* m_gravity;
    float m_particleSpread;
    float m_particleMass;
    quint32 m_maxParticles;
};

#endif // COFFEETRANSFORMCOMPUTER_H
