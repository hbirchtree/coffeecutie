#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include <QObject>
#include "engine/objects/coffeeobject.h"
#include "general/data/numberbuffer.h"

class ShaderContainer;
class CoffeeTexture;
class CoffeeCamera;

class CoffeeParticleSystem : public CoffeeObject
{
    Q_PROPERTY(quint64 processTime READ processTime)
    Q_PROPERTY(quint64 particleCount READ particleCount)
    Q_PROPERTY(QColor particleColor READ particleColor WRITE setParticleColor)
    Q_PROPERTY(float particleSize READ particleSize WRITE setParticleSize)
    Q_PROPERTY(quint32 max_particles READ maxParticles WRITE setMaxParticles)

    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)

//    Q_OBJECT

public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    void render();
    void unload();
    void load();

    bool isBaked();
    void setBaked(bool val);

    quint32 getMaxParticles() const;
    float getParticleSize() const;
    QColor getParticleColor() const;
    quint64 getParticleCount() const;
    quint64 getProcessTime() const;

public slots:
    void setFrametime(float time);

    void setMaxParticles(quint32 max_particles);
    void setParticleSize(float particleSize);
    void setParticleColor(QColor particleColor);

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

    quint32 active_particles = 1;
    quint32 max_particles = 10240;

    quint32 spawncount = 20;

    GLuint64 processtime = 0;
};

#endif // COFFEEPARTICLESYSTEM_H
