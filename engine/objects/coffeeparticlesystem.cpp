#include "coffeeparticlesystem.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "opengl/helpers/renderingmethods.h"
#include "engine/compute/coffeetransformcomputer.h"
#include <QColor>

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) :
    CoffeeObject(parent)
{
    this->m_gravity = new Vector3Value(this,glm::vec3(0,-1,0));
    this->transform = new CoffeeTransformComputer(this);
    this->m_camera = camera;

    CoffeeTransformComputer::Particle first;
    first.type = 0;
    first.pos = glm::vec3(0,0,0);
    first.vel = glm::vec3(0,10,0);
    first.lifetime = 6.f;

    position()->setValue(glm::vec3(5,0,5));

    transform->getParticles()->append(first);

    m_particleSize = new ScalarValue(this,0.5f);
    m_frametime = new ScalarValue(this,0.1f);
    m_particleMass = new ScalarValue(this,1.0f);
    m_particleSpread = new ScalarValue(this,0.1f);
    m_randomDir = new ScalarValue(this,[=](float v){
        Q_UNUSED(v)
        return (float)(qrand()%1256000-628000)/100000.f;
    });
    m_randomAmplitude = new ScalarValue(this,[=](float v){
        Q_UNUSED(v)
        return (float)(qrand()%1000000-500000)/500000.f;
    });
    m_spawnCount = new ScalarValue(this,1.f);
    transform->setUniform("timestep",m_frametime);
    transform->setUniform("mass",m_particleMass);
    transform->setUniform("gravity",m_gravity);
    transform->setUniform("randRad",m_randomDir);
    transform->setUniform("randAmpDiff",m_randomAmplitude);
    transform->setUniform("partSpread",m_particleSpread);
    transform->setUniform("spawncount",m_spawnCount);
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
    m_shader->unload();
    texture->unloadTexture();
    transform->unload();

    setBaked(false);
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();
    m_shader->buildProgram();
    transform->load();

    setBaked(true);
}

void CoffeeParticleSystem::setFrametime(float time)
{
    *m_frametime = time;
}

bool CoffeeParticleSystem::isBaked()
{
    return baked;
}

void CoffeeParticleSystem::setBaked(bool val)
{
    this->baked = val;
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

    glUseProgram(m_shader->getProgramId());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture->getGlTextureType(),texture->getHandle());

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,transform->getRenderTransform());
    glBindVertexArray(transform->getRenderArray());

    m_shader->setUniform("colorMultiplier",m_particleColor);
    m_shader->setUniform("diffuseSampler",0);
    m_shader->setUniform("modelview",RenderingMethods::translateObjectMatrix(
                           position()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue())*m_camera->getMatrix());
    m_shader->setUniform("cameraPos",m_camera->getCameraPos());
    m_shader->setUniform("particleSize",m_particleSize);

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

QObject *CoffeeParticleSystem::gravityQObject()
{
    return m_gravity;
}

QObject *CoffeeParticleSystem::particleColor()
{
    return m_particleColor;
}

QObject *CoffeeParticleSystem::particleSpread()
{
    return m_particleSpread;
}

QObject *CoffeeParticleSystem::shader() const
{
    return m_shader;
}

QObject *CoffeeParticleSystem::particleSize()
{
    return m_particleSize;
}

QObject *CoffeeParticleSystem::particleMass()
{
    return m_particleMass;
}

QPointer<CoffeeShader> CoffeeParticleSystem::getShader()
{
    return m_shader;
}

void CoffeeParticleSystem::setShader(CoffeeShader *value)
{
    if(m_shader)
        m_shader->removeConsumer();
    m_shader = value;
    m_shader->addConsumer();
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

void CoffeeParticleSystem::setShader(QObject *value)
{
    CoffeeShader* s = qobject_cast<CoffeeShader*>(value);
    if(s)
        setShader(s);
}

void CoffeeParticleSystem::setGravity(Vector3Value *gravity)
{
    m_gravity = gravity;
}

void CoffeeParticleSystem::setGravity(QObject *gravity)
{
    Vector3Value* v = qobject_cast<Vector3Value*>(gravity);
    if(v)
        setGravity(v);
}

void CoffeeParticleSystem::setCamera(const QObject *value)
{
    const CoffeeCamera* c = qobject_cast<const CoffeeCamera*>(value);
    if(c)
        setCamera(c);
}

void CoffeeParticleSystem::setCamera(const CoffeeCamera *value)
{
    m_camera = value;
}
