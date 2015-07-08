#ifndef COFFEETRANSFORMCOMPUTER_H
#define COFFEETRANSFORMCOMPUTER_H

#include "general/common.h"

class ShaderContainer;
class Vector3Value;
class ShaderVariant;
class ShaderMapping;

class CoffeeTransformComputer : public QObject
{
    Q_PROPERTY(float particleSpread READ particleSpread WRITE setParticleSpread)
    Q_PROPERTY(float particleMass READ particleMass WRITE setParticleMass)
    Q_PROPERTY(quint32 maxParticles READ maxParticles WRITE setMaxParticles)
    Q_PROPERTY(QObject* gravity READ getGravityObject)
    Q_PROPERTY(QVariantList feedbackAttributes READ feedbackAttributes WRITE setFeedbackAttributes)

    Q_PROPERTY(uint activeParticles READ activeParticles)
    Q_PROPERTY(quint64 processTime READ processTime)
    Q_PROPERTY(uint spawnCount READ spawnCount)

    Q_PROPERTY(bool query READ query WRITE setQuery)

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

    ShaderContainer *getShader();

    QVariantList feedbackAttributes() const;

    uint activeParticles() const;
    uint spawnCount() const;
    quint64 processTime() const;

    bool query() const;

public slots:
    void doReload();

    void load();
    void unload();
    void tickParticles();

    void setShader(ShaderContainer* shader);
    void setParticleSpread(float particleSpread);
    void setParticleMass(float particleMass);
    void setMaxParticles(quint32 maxParticles);
    void setGravity(const glm::vec3 &grav);

    void setUniform(QString uniformName, ShaderVariant *data);
    void setFeedbackAttributes(QVariantList feedbackAttributes);

    void setQuery(bool query);

private slots:
    void switchIndex();

private:
    uint vboIndex();
    uint vaoIndex();

    bool m_reload = false;
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
    GLuint64 m_processTime = 0;

    QVector<Particle> startParticles;
    QVector<ShaderMapping*> uniforms;
    Vector3Value* m_gravity;
    float m_particleSpread = 0.f;
    float m_particleMass = 1.0f;
    quint32 m_maxParticles = 1024;
    QVariantList m_feedbackAttributes;
    bool m_query;
    bool t_query;
};

#endif // COFFEETRANSFORMCOMPUTER_H
