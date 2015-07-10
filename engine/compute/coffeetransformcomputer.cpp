#include "coffeetransformcomputer.h"

#include "opengl/components/shadercontainer.h"
#include "general/shadervariant.h"
#include "engine/objects/coffeestandardobject.h"

CoffeeTransformComputer::CoffeeTransformComputer(QObject *parent) : QObject(parent)
{
    this->m_gravity = new Vector3Value(this,glm::vec3(0,-1,0));
}

QObject *CoffeeTransformComputer::getGravityObject()
{
    return m_gravity;
}

glm::vec3 CoffeeTransformComputer::gravity() const
{
    return m_gravity->getValue();
}

float CoffeeTransformComputer::particleSpread() const
{
    return m_particleSpread;
}

float CoffeeTransformComputer::particleMass() const
{
    return m_particleMass;
}

quint32 CoffeeTransformComputer::maxParticles() const
{
    return m_maxParticles;
}

GLuint CoffeeTransformComputer::getRenderTransform()
{
    return tfbs[vboIndex()];
}

GLuint CoffeeTransformComputer::getRenderArray()
{
    return vaos[vboIndex()];
}

QVector<CoffeeTransformComputer::Particle> *CoffeeTransformComputer::getParticles()
{
    return &startParticles;
}

ShaderContainer *CoffeeTransformComputer::getShader()
{
    return tshader;
}

QVariantList CoffeeTransformComputer::feedbackAttributes() const
{
    return m_feedbackAttributes;
}

uint CoffeeTransformComputer::activeParticles() const
{
    return active_particles;
}

uint CoffeeTransformComputer::spawnCount() const
{
    return spawncount;
}

quint64 CoffeeTransformComputer::processTime() const
{
    return m_processTime;
}

bool CoffeeTransformComputer::query() const
{
    return m_query;
}

bool CoffeeTransformComputer::capture() const
{
    return m_capture;
}

void CoffeeTransformComputer::doReload()
{
    m_reload = true;
}

void CoffeeTransformComputer::tickParticles()
{
    if(m_reload){
        unload();
        load();
    }
    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vaos[vaoIndex()]);

    if(active_particles+spawncount>maxParticles()-1)
        spawncount = 0;
    else if(active_particles<maxParticles())
        spawncount = 1;

    for(ShaderMapping* m : uniforms){
        tshader->setUniform(m->uniform,m->data);
    }

    t_query = m_query;
    if(t_query){
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,primitiveQuery);
        glBeginQuery(GL_TIME_ELAPSED,timeQuery);
    }

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[vaoIndex()]);
    glBeginTransformFeedback(GL_POINTS);

    if(!m_started){
        glDrawArrays(GL_POINTS,0,active_particles);
        m_started = true;
    }else
        glDrawTransformFeedback(GL_POINTS,tfbs[vboIndex()]);

    glEndTransformFeedback();

    if(t_query){
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        glEndQuery(GL_TIME_ELAPSED);

        //getting these values tanks performance!
        glGetQueryObjectuiv(primitiveQuery,GL_QUERY_RESULT,&active_particles);
        glGetQueryObjectui64v(timeQuery,GL_QUERY_RESULT,&m_processTime);
    }

    glDisable(GL_RASTERIZER_DISCARD);

    switchIndex();
}

void CoffeeTransformComputer::setShader(ShaderContainer *shader)
{
    this->tshader = shader;
}

void CoffeeTransformComputer::setParticleSpread(float particleSpread)
{
    m_particleSpread = particleSpread;
}

void CoffeeTransformComputer::setParticleMass(float particleMass)
{
    m_particleMass = particleMass;
}

void CoffeeTransformComputer::setMaxParticles(quint32 maxParticles)
{
    m_maxParticles = maxParticles;
}

void CoffeeTransformComputer::setGravity(const glm::vec3 &grav)
{
    this->m_gravity->setValue(grav);
}

void CoffeeTransformComputer::switchIndex()
{
    index = (index+1)%2;
}

void CoffeeTransformComputer::load()
{
    index = 0;
    tshader->createProgram();
    tshader->compileShaders();

    QVector<std::string> feedback_backing; //we need to do this in order for the memory to be retained
    QVector<const char*> feedbackattributes;
    for(QVariant s : this->m_feedbackAttributes){
        std::string t = s.toString().toStdString();
        feedbackattributes.append(t.c_str());
        feedback_backing.append(t);
    }
    glTransformFeedbackVaryings(tshader->getProgramId(),
                                feedbackattributes.size(),
                                feedbackattributes.data(),
                                GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    QVector<Particle> partsbuffer = QVector<Particle>(startParticles);
    partsbuffer.resize(maxParticles());

    //Two buffers are swapped to provide iteration on the data

    glBindVertexArray(vaos[vaoIndex()]);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vaoIndex()]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*maxParticles(), partsbuffer.data(), GL_STATIC_DRAW);

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

    glBindVertexArray(vaos[vboIndex()]);

    glBindBuffer(GL_ARRAY_BUFFER,vbos[vboIndex()]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(Particle)*maxParticles(),nullptr,GL_STATIC_READ);

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
    }
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);

    glGenQueries(1,&timeQuery);
    glGenQueries(1,&primitiveQuery);

    tickParticles();
}

void CoffeeTransformComputer::unload()
{
    index = 0;
    tshader->unload();
    glDeleteTransformFeedbacks(2,tfbs);
    glDeleteVertexArrays(2,vaos);
    glDeleteBuffers(2,vbos);
    glDeleteQueries(1,&timeQuery);
    glDeleteQueries(1,&primitiveQuery);
}

uint CoffeeTransformComputer::vboIndex()
{
    return (index+1)%2;
}

uint CoffeeTransformComputer::vaoIndex()
{
    return index;
}

void CoffeeTransformComputer::setUniform(QString uniformName, ShaderVariant* data)
{
    ShaderMapping *map = new ShaderMapping;
    map->uniform = uniformName;
    map->data = data;
    uniforms.append(map);
}

void CoffeeTransformComputer::setFeedbackAttributes(const QVariantList &feedbackAttributes)
{
    m_feedbackAttributes = feedbackAttributes;
}

void CoffeeTransformComputer::setQuery(bool query)
{
    m_query = query;
    if(!query){
        m_processTime = 0;
        active_particles = 0;
    }
}

void CoffeeTransformComputer::setCapture(bool capture)
{
    m_capture = capture;
}
