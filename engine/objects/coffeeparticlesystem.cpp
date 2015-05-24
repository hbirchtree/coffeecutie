#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeSimpleObject(parent)
{
    this->camera = camera;
    shader = new ShaderContainer(this);
    shader->buildProgram("ubw/shaders/particles/Particle.vsh","ubw/shaders/particles/Particle.fsh");

    shader->getUniformLocation("CameraRight_worldspace");
    shader->getUniformLocation("CameraUp_worldspace");
    shader->getUniformLocation("VP");

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
    };

    pos = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
}

glm::vec3 CoffeeParticleSystem::getPosition() const
{
    return pos->getValue();
}

QPointer<NumberContainer<glm::vec3> > CoffeeParticleSystem::getPositionObject()
{
    return pos;
}

void CoffeeParticleSystem::setPosition(const glm::vec3 &pos)
{
    this->pos->setValue(pos);
}

void CoffeeParticleSystem::loadTransformAttributes()
{
    for(int i=0;i<transformVariables.size();i++){
        glTransformFeedbackVaryings(tshader->getProgramId(),transformVariables.size(),
                                    transformVariables.data(),GL_INTERLEAVED_ATTRIBS);
    }
}
