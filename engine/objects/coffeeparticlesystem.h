#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include <QObject>
#include "engine/objects/coffeesimpleobject.h"
#include "opengl/components/coffeecamera.h"

class CoffeeParticleSystem : public CoffeeSimpleObject
{
public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    glm::vec3 getPosition() const;
    QPointer<NumberContainer<glm::vec3>> getPositionObject();
    void setPosition(const glm::vec3& pos);


    void render(); //This is the part where the visible part is shown

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

    void clearParticles();
    qint64 getCountParticles();

protected:
    enum ParticleTypes {
        ParticleEmittedType,
        ParticleGeneratorType
    };

    //Particle transform
    ShaderContainer* tshader;

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

    QPointer<NumberContainer<glm::vec3>> pos;
};

#endif // COFFEEPARTICLESYSTEM_H
