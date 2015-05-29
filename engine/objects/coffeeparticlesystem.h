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

    void setProperties(
            glm::vec3 sourcePos,
            glm::vec3 minVelocity,
            glm::vec3 maxVelocity,
            glm::vec3 gravity,
            glm::vec3 color,
            float minLifetime,
            float maxLifetime,
            float size,
            float renewTime,
            float particles);

    void updateParticles(float timeStep);

    bool isBaked();
    void setBaked(bool val);

    void clearParticles();
    qint64 getCountParticles();

protected:
    bool baked = false;
    enum ParticleTypes {
        ParticleEmittedType,
        ParticleGeneratorType
    };

    QPointer<ShaderContainer> shader;
    //Particle transform
    QPointer<ShaderContainer> tshader;

    GLuint transformBuffer = 0;
    GLuint transformQuery = 0;

    GLuint partsBuffers[2];
    GLuint partsArrays[2];

    struct Particle {
        //particle attributes, loaded into GPU memory
        glm::vec3 pos;
        glm::vec3 vel;
        glm::vec3 col;
        GLfloat size;
        GLfloat life;
        GLfloat cameraDist;
        GLint type;
    };
    GLint parts_max_count = 16384;
    GLint parts_curr_count = 1;
    GLint parts_gen_count = 100;

    glm::vec3 parts_ivel;
    glm::vec3 parts_rvel;
    glm::vec3 parts_gravity;
    glm::vec3 parts_color;
    glm::vec3 parts_src_pos;

    CoffeeTexture* texture;

    float parts_time = 0.f;
    float parts_renewtime = 500.f;

    float part_life_min = 10.f;
    float part_life_range = 0.f;
    float part_size = 1.0f;

    int curReadBuffer = 0;

    void setupSystem();
    glm::vec3 genRandVec3(float base, float range);
    float genRandF(float base, float range);

    const CoffeeCamera* camera;
};

#endif // COFFEEPARTICLESYSTEM_H
