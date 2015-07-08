#include "coffeetransformcomputer.h"

#include "opengl/components/shadercontainer.h"
#include "engine/objects/coffeeobject.h"

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

void CoffeeTransformComputer::tickParticles(float frametime)
{
    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vaos[vaoIndex()]);

    tshader->setUniform("timestep",frametime);

//    if(active_particles+spawncount>max_particles-1)
//        spawncount = 0;
//    else if(active_particles<max_particles)
//        spawncount = 1;

    //    qDebug("Spawn count: %u",spawncount);

    tshader->setUniform("mass",particleMass());
    tshader->setUniform("gravity",gravity());
    tshader->setUniform("randRad",(float)(qrand()%1256000-628000)/100000.f);
    tshader->setUniform("randAmpDiff",(float)(qrand()%1000000-500000)/500000.f);
    tshader->setUniform("partSpread",particleSpread());
    tshader->setUniform("spawncount",(float)spawncount);

//    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,primitiveQuery);
//    glBeginQuery(GL_TIME_ELAPSED,timeQuery);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[vaoIndex()]);
    glBeginTransformFeedback(GL_POINTS);

    if(!m_started){
        glDrawArrays(GL_POINTS,0,active_particles);
        m_started = true;
    }else
        glDrawTransformFeedback(GL_POINTS,tfbs[vboIndex()]);

    glEndTransformFeedback();

//    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
//    glEndQuery(GL_TIME_ELAPSED);

    //getting these values tanks performance!
//    glGetQueryObjectuiv(primitiveQuery,GL_QUERY_RESULT,&active_particles);
//    glGetQueryObjectui64v(timeQuery,GL_QUERY_RESULT,&processtime);

//    qDebug("Particle data: %u written",active_particles);

    glDisable(GL_RASTERIZER_DISCARD);
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

void CoffeeTransformComputer::switchIndex()
{
    index = (index+1)%2;
}

void CoffeeTransformComputer::load()
{
    index = 0;
    tshader->createProgram();
    tshader->compileShaders();

    const GLchar* feedbackattributes[] = {"outType","outPos","outVel","outLife"};
    glTransformFeedbackVaryings(tshader->getProgramId(),4,feedbackattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    glBindVertexArray(vaos[vaoIndex()]);

    QVector<Particle> partsbuffer = QVector<Particle>(startParticles);
    partsbuffer.resize(maxParticles());

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

    glBindVertexArray(0);

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

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
    }

    glGenQueries(1,&timeQuery);
    glGenQueries(1,&primitiveQuery);

    tickParticles(0);
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
