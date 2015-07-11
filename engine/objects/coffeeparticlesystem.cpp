#include "coffeeparticlesystem.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "opengl/helpers/renderingmethods.h"
#include "general/shadervariant.h"
#include "engine/compute/coffeetransformcomputer.h"
#include <QColor>

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) :
    QObject(parent),
    CoffeeObject(this)
{
    this->transform = new CoffeeTransformComputer(this);
    this->camera = camera;

    CoffeeTransformComputer::Particle first;
    first.type = 0;
    first.pos = glm::vec3(0,0,0);
    first.vel = glm::vec3(0,10,0);
    first.lifetime = 6.f;

    position()->setValue(glm::vec3(5,0,5));

    transform->getParticles()->append(first);

    frametime = new ShaderVariant([=](){
        return tickTime;
    });
    mass = new ShaderVariant([=](){
        return transform->particleMass();
    });
    partSpread = new ShaderVariant([=](){
        return transform->particleSpread();
    });
    gravity = new ShaderVariant([=](){
        return transform->gravity();
    });
    randRad = new ShaderVariant([=](){
        return (float)(qrand()%1256000-628000)/100000.f;
    });
    randAmp = new ShaderVariant([=](){
        return (float)(qrand()%1000000-500000)/500000.f;
    });
    spawncount = new ShaderVariant([=](){
        return 1.f;
    });
    transform->setUniform("timestep",frametime);
    transform->setUniform("mass",mass);
    transform->setUniform("gravity",gravity);
    transform->setUniform("randRad",randRad);
    transform->setUniform("randAmpDiff",randAmp);
    transform->setUniform("partSpread",partSpread);
    transform->setUniform("spawncount",spawncount);
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    transform->tickParticles();
    renderParticles();
}

void CoffeeParticleSystem::unload()
{
    shader->unload();
    texture->unloadTexture();
    transform->unload();

    setBaked(false);
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();
    shader->buildProgram();
    transform->load();

    setBaked(true);
}

void CoffeeParticleSystem::setFrametime(float time)
{
    this->tickTime = time;
}

bool CoffeeParticleSystem::isBaked()
{
    return baked;
}

void CoffeeParticleSystem::setBaked(bool val)
{
    this->baked = val;
}

float CoffeeParticleSystem::getParticleSize() const
{
    return particleSize;
}

QColor CoffeeParticleSystem::getParticleColor() const
{
    glm::vec4 c = particleColor*255.f;
    return QColor(c.x,c.y,c.z,c.w);
}

void CoffeeParticleSystem::setParticleSize(float particleSize)
{
    this->particleSize = particleSize;
}

void CoffeeParticleSystem::setParticleColor(QColor particleColor)
{
    this->particleColor = glm::vec4(
                particleColor.redF(),
                particleColor.greenF(),
                particleColor.blueF(),
                particleColor.alphaF());
}

void CoffeeParticleSystem::setAdditive(bool additive)
{
    m_additive = additive;
}

void CoffeeParticleSystem::renderParticles()
{
    c_additive = m_additive;

    if(c_additive)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glDepthMask(GL_FALSE);

    glUseProgram(shader->getProgramId());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture->getGlTextureType(),texture->getHandle());

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,transform->getRenderTransform());
    glBindVertexArray(transform->getRenderArray());

    shader->setUniform("colorMultiplier",particleColor);
    shader->setUniform("diffuseSampler",0);
    shader->setUniform("modelview",RenderingMethods::translateObjectMatrix(
                           position()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue())*camera->getMatrix());
    shader->setUniform("cameraPos",camera->getCameraPos());
    shader->setUniform("particleSize",particleSize);

    glDrawTransformFeedback(GL_POINTS,transform->getRenderTransform());

    glBindTexture(texture->getGlTextureType(),0);
    glBindVertexArray(0);
    glUseProgram(0);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);

    glDepthMask(GL_TRUE);
    if(c_additive)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

bool CoffeeParticleSystem::additive() const
{
    return m_additive;
}

QObject *CoffeeParticleSystem::getTransformObject()
{
    return transform;
}

QObject* CoffeeParticleSystem::getTextureQObject()
{
    return texture;
}

QPointer<CoffeeShader> CoffeeParticleSystem::getShader()
{
    return shader;
}

void CoffeeParticleSystem::setShader(QPointer<CoffeeShader> value)
{
    if(shader)
        shader->removeConsumer();
    shader = value;
    shader->addConsumer();
}

CoffeeTransformComputer *CoffeeParticleSystem::getTransform()
{
    return transform;
}

QPointer<CoffeeTexture> CoffeeParticleSystem::getTexture()
{
    return texture;
}

void CoffeeParticleSystem::setTexture(CoffeeTexture *value)
{
    if(texture)
        texture->removeConsumer();
    texture = value;
    texture->addConsumer();
}

void CoffeeParticleSystem::setTexture(QObject *value)
{
    CoffeeTexture* tex = qobject_cast<CoffeeTexture*>(value);
    if(tex)
        setTexture(tex);
}

void CoffeeParticleSystem::setCamera(const CoffeeCamera *value)
{
    camera = value;
}
