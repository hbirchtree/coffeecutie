#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include <QObject>
#include "engine/objects/coffeeobject.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/coffeecamera.h"

class CoffeeParticleSystem : public CoffeeObject
{
    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)
public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    void render(); //This is the part where the visible part is shown
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
    QPointer<CoffeeTexture> randTexture;
    const CoffeeCamera* camera;

    enum ParticleType {
        EmitterParticleType,
        EmittedParticleType
    };

    struct Particle {
        float type;
        glm::vec3 pos;
        glm::vec3 vel;
        glm::vec3 acc;
        float lifetime;
    };

    bool started = false;
    uint vbIndex = 0;
    uint tfIndex = 0;

    float particleSize = 0.01f;

    GLuint vaos_r[2];
    GLuint vaos_t[2];
    GLuint vbos[2];
    GLuint tfbs[2];

    float time = 0;

    quint32 max_particles = 2048;
};

#endif // COFFEEPARTICLESYSTEM_H
