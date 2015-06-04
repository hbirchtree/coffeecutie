#include "coffeeparticlesystem.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "opengl/helpers/renderingmethods.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeObject(parent)
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

    startParticles.append(first);
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vaos[vaoIndex]);

    tshader->setUniform("timestep",frametime);

    if(active_particles+spawncount>max_particles-1)
        spawncount = 0;
    else if(active_particles+1<max_particles-1)
        spawncount = 1;

//    qDebug("Spawn count: %u",spawncount);

    tshader->setUniform("rand",(float)(qrand()%1000000-500000)/100000.f);

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

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader->getProgramId());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture->getHandle());

    shader->setUniform("colorMultiplier",particleColor);
    shader->setUniform("diffuseSampler",0);
    shader->setUniform("modelview",camera->getMatrix()*RenderingMethods::translateObjectMatrix(
                           position()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue()));
    shader->setUniform("cameraPos",camera->getCameraPos());
    shader->setUniform("particleSize",particleSize);

    glDrawTransformFeedback(GL_POINTS,tfbs[tfbIndex]);

    glBindVertexArray(0);
    glUseProgram(0);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    tfbIndex = (tfbIndex+1)%2;

    vaoIndex = vboIndex;
    vboIndex = (vboIndex+1)%2;
}

void CoffeeParticleSystem::unload()
{
    texture->unloadTexture();
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();

//    shader->buildProgram("ubw/shaders/particles/billboard.vs",
//                         "ubw/shaders/particles/billboard.fs",
//                         "ubw/shaders/particles/billboard.gs");
    shader->buildProgram();

    glUseProgram(shader->getProgramId());

    shader->getUniformLocation("colorMultiplier");
    shader->getUniformLocation("diffuseSampler");
    shader->getUniformLocation("modelview");
    shader->getUniformLocation("cameraPos");
    shader->getUniformLocation("particleSize");

    tshader->createProgram();

//    tshader->setVertexShader("ubw/shaders/particles/vertex-process.vert");
//    tshader->setGeometryShader("ubw/shaders/particles/geometry-process.geom");

    tshader->compileShaders();

    const GLchar* feedbackattributes[] = {"outType","outPos","outVel","outLife"};
    glTransformFeedbackVaryings(tshader->getProgramId(),4,feedbackattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glUseProgram(tshader->getProgramId());

    tshader->getUniformLocation("mass");
    tshader->getUniformLocation("spawncount");
    tshader->getUniformLocation("timestep");
    tshader->getUniformLocation("gravity");
    tshader->getUniformLocation("rand");
    tshader->setUniform("mass",particleMass());
    tshader->setUniform("gravity",gravity());

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    glBindVertexArray(vaos[vaoIndex]);

    QVector<Particle> partsbuffer = QVector<Particle>(startParticles);
    partsbuffer.resize(max_particles);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vaoIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*max_particles, partsbuffer.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inType"));
    glVertexAttribPointer(tshader->getAttributeLocation("inType"),1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)0);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inPos"));
    glVertexAttribPointer(tshader->getAttributeLocation("inPos"),3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)4);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inVel"));
    glVertexAttribPointer(tshader->getAttributeLocation("inVel"),3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)16);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inLife"));
    glVertexAttribPointer(tshader->getAttributeLocation("inLife"),1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)28);

    glBindVertexArray(0);

    glBindVertexArray(vaos[vboIndex]);

    glBindBuffer(GL_ARRAY_BUFFER,vbos[vboIndex]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Particle)*max_particles,nullptr,GL_STATIC_READ);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inType"));
    glVertexAttribPointer(tshader->getAttributeLocation("inType"),1,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)0);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inPos"));
    glVertexAttribPointer(tshader->getAttributeLocation("inPos"),3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)4);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inVel"));
    glVertexAttribPointer(tshader->getAttributeLocation("inVel"),3,GL_FLOAT,
                          GL_FALSE,sizeof(Particle),(GLvoid*)16);

    glEnableVertexAttribArray(tshader->getAttributeLocation("inLife"));
    glVertexAttribPointer(tshader->getAttributeLocation("inLife"),1,GL_FLOAT,
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
    frametime = time*2;
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
    qDebug() << particleColor;
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
QPointer<ShaderContainer> CoffeeParticleSystem::getTransformShader()
{
    return tshader;
}

void CoffeeParticleSystem::setTransformShader(QPointer<ShaderContainer> value)
{
    tshader = value;
}

QPointer<ShaderContainer> CoffeeParticleSystem::getShader()
{
    return shader;
}

void CoffeeParticleSystem::setShader(QPointer<ShaderContainer> value)
{
    shader = value;
}

QPointer<CoffeeTexture> CoffeeParticleSystem::getTexture()
{
    return texture;
}

void CoffeeParticleSystem::setTexture(QPointer<CoffeeTexture> value)
{
    texture = value;
}

void CoffeeParticleSystem::setCamera(const CoffeeCamera *value)
{
    camera = value;
}

