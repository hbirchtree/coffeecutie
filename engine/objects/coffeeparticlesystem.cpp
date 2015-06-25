#include "coffeeparticlesystem.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "opengl/helpers/renderingmethods.h"
#include <QColor>

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) :
    QObject(parent),
    CoffeeObject(this)
{
    this->camera = camera;

//    particleColor = glm::vec4(0.5,0.5,0.5,0.9);

//    tshader = new ShaderContainer(this);
//    shader = new ShaderContainer(this);

//    texture = new CoffeeTexture(this,"ubw/models/textures/particle_fx.png");
//    texture->setObjectName("sprite");

    Particle first;
    first.type = 0;
    first.pos = glm::vec3(0,0,0);
    first.vel = glm::vec3(0,10,0);
    first.lifetime = 6.f;

    position()->setValue(glm::vec3(5,0,5));

    startParticles.append(first);
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    tickParticles(0.0005f);
    renderParticles();
//    particleTicks.remove(0,i);
}

void CoffeeParticleSystem::unload()
{
    shader->unload();
    tshader->unload();
    glDeleteTransformFeedbacks(2,tfbs);
    glDeleteVertexArrays(2,vaos);
    glDeleteBuffers(2,vbos);
    glDeleteQueries(1,&timeQuery);
    glDeleteQueries(1,&primitiveQuery);
    texture->unloadTexture();

    vaoIndex = 0;
    vboIndex = 1;
    tfbIndex = 0;
    active_particles = 1;

    setBaked(false);
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();

    //Rendering
    shader->buildProgram();

    //Transform
    tshader->createProgram();
    tshader->compileShaders();

    const GLchar* feedbackattributes[] = {"outType","outPos","outVel","outLife"};
    glTransformFeedbackVaryings(tshader->getProgramId(),4,feedbackattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glUseProgram(tshader->getProgramId());
    tshader->setUniform("mass",particleMass());
    tshader->setUniform("gravity",gravity());

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    glBindVertexArray(vaos[vaoIndex]);

    QVector<Particle> partsbuffer = QVector<Particle>(startParticles);
    partsbuffer.resize(max_particles);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vaoIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*max_particles, partsbuffer.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)4);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)16);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)28);

    glBindVertexArray(0);

    glBindVertexArray(vaos[vboIndex]);

    glBindBuffer(GL_ARRAY_BUFFER,vbos[vboIndex]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Particle)*max_particles,nullptr,GL_STATIC_READ);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)4);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)16);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)28);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenTransformFeedbacks(2,tfbs);

    for(int i=0;i<2;i++){
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[i]);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,vbos[(i+1)%2]);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
    }

    glGenQueries(1,&timeQuery);
    glGenQueries(1,&primitiveQuery);

    tickParticles(0.001);

    setBaked(true);
}

bool CoffeeParticleSystem::isBaked()
{
    return baked;
}

void CoffeeParticleSystem::setBaked(bool val)
{
    this->baked = val;
}

quint32 CoffeeParticleSystem::getMaxParticles() const
{
    return max_particles;
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

quint64 CoffeeParticleSystem::getParticleCount() const
{
    return active_particles;
}

quint64 CoffeeParticleSystem::getProcessTime() const
{
    return processtime;
}

glm::vec3 CoffeeParticleSystem::gravity() const
{
    return m_gravity;
}

float CoffeeParticleSystem::particleMass() const
{
    return m_particleMass;
}

void CoffeeParticleSystem::setFrametime(float time)
{
    particleTicks.append(time);
}

void CoffeeParticleSystem::setMaxParticles(quint32 max_particles)
{
    this->max_particles = max_particles;
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

void CoffeeParticleSystem::setGravity(const glm::vec3 &gravity)
{
    m_gravity = gravity;
}

void CoffeeParticleSystem::setParticleMass(float particleMass)
{
    m_particleMass = particleMass;
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
    glBindTexture(GL_TEXTURE_2D,texture->getHandle());

    shader->setUniform("colorMultiplier",particleColor);
    shader->setUniform("diffuseSampler",0);
    shader->setUniform("modelview",RenderingMethods::translateObjectMatrix(
                           position()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue())*camera->getMatrix());
    shader->setUniform("cameraPos",camera->getCameraPos());
    shader->setUniform("particleSize",particleSize);

    glDrawTransformFeedback(GL_POINTS,tfbs[tfbIndex]);

    glBindVertexArray(0);
    glUseProgram(0);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);

    glDepthMask(GL_TRUE);
    if(c_additive)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void CoffeeParticleSystem::tickParticles(float frametime)
{
    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vaos[vaoIndex]);

    tshader->setUniform("timestep",frametime);

    if(active_particles+spawncount>max_particles-1)
        spawncount = 0;
    else if(active_particles<max_particles)
        spawncount = 1;

//    qDebug("Spawn count: %u",spawncount);

    tshader->setUniform("randRad",(float)(qrand()%1256000-628000)/100000.f);
    tshader->setUniform("randAmpDiff",(float)(qrand()%1000000-500000)/500000.f);

    tshader->setUniform("spawncount",(float)spawncount);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,primitiveQuery);
//    glBeginQuery(GL_TIME_ELAPSED,timeQuery);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[tfbIndex]);
    glBeginTransformFeedback(GL_POINTS);

    if(!started){
        glDrawArrays(GL_POINTS,0,active_particles);
        started = true;
    }else
        glDrawTransformFeedback(GL_POINTS,tfbs[(tfbIndex+1)%2]);

    glEndTransformFeedback();

    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
//    glEndQuery(GL_TIME_ELAPSED);

    //getting these values tanks performance!
//    glGetQueryObjectuiv(primitiveQuery,GL_QUERY_RESULT,&active_particles);
//    glGetQueryObjectui64v(timeQuery,GL_QUERY_RESULT,&processtime);

//    qDebug("Particle data: %u written",active_particles);

    glDisable(GL_RASTERIZER_DISCARD);

    tfbIndex = (tfbIndex+1)%2;
    vaoIndex = vboIndex;
    vboIndex = (vboIndex+1)%2;
}
QPointer<ShaderContainer> CoffeeParticleSystem::getTransformShader()
{
    return tshader;
}

void CoffeeParticleSystem::setTransformShader(QPointer<ShaderContainer> value)
{
    if(tshader)
        tshader->removeConsumer();
    tshader = value;
    tshader->addConsumer();
}

bool CoffeeParticleSystem::additive() const
{
    return m_additive;
}

QPointer<ShaderContainer> CoffeeParticleSystem::getShader()
{
    return shader;
}

void CoffeeParticleSystem::setShader(QPointer<ShaderContainer> value)
{
    if(shader)
        shader->removeConsumer();
    shader = value;
    shader->addConsumer();
}

QPointer<CoffeeTexture> CoffeeParticleSystem::getTexture()
{
    return texture;
}

void CoffeeParticleSystem::setTexture(QPointer<CoffeeTexture> value)
{
    if(texture)
        texture->removeConsumer();
    texture = value;
    texture->addConsumer();
}

void CoffeeParticleSystem::setCamera(const CoffeeCamera *value)
{
    camera = value;
}
