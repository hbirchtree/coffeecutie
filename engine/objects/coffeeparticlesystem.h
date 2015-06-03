#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include <QObject>
#include "engine/objects/coffeeobject.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/coffeecamera.h"

#define QUERY_PRIMITIVES_WRITTEN 0
#define QUERY_TIME_ELAPESED 1

class CoffeeParticleSystem : public CoffeeObject
{
    Q_PROPERTY(quint64 processTime READ processTime)
    Q_PROPERTY(quint64 particleCount READ particleCount)

    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)
public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    void render();
    void unload();
    void load();

    bool isBaked();
    void setBaked(bool val);

public slots:
    void tick(float delta);

protected:

    void updateParticles(float delta);

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

    float frametime = 0.005f;

    uint vaoIndex = 0;
    uint vboIndex = 1;

    GLuint vaos[2];
    GLuint vbos[2];

    GLuint timeQuery;
    GLuint primitiveQuery;
    GLuint queries[2];

    float particleSize = 0.01f;

    quint32 active_particles = 1;
    quint32 max_particles = 2048;

    quint32 spawncount = 20;

    GLuint64 primitives = 0;
    GLuint64 processtime = 0;
};

#endif // COFFEEPARTICLESYSTEM_H
