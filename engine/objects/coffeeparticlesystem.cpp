#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeObject(parent)
{
    this->camera = camera;


    tshader = new ShaderContainer(this);
    shader = new ShaderContainer(this);

    texture = new CoffeeTexture(this,"ubw/models/textures/quadtex.png");
    texture->setObjectName("sprite");
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vaos[vaoIndex]);

    tshader->setUniform("timestep",frametime);

    if(active_particles+spawncount>max_particles)
        spawncount = std::min((quint32)0,max_particles-active_particles);
    else
        spawncount = 20;

    tshader->setUniform("spawncount",(float)spawncount);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,primitiveQuery);
//    glBeginQuery(GL_TIME_ELAPSED,timeQuery);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,vbos[vboIndex]);
    glBeginTransformFeedback(GL_POINTS);

    glDrawArrays(GL_POINTS,0,active_particles);

    glEndTransformFeedback();

    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
//    glEndQuery(GL_TIME_ELAPSED);

    //getting these values tanks performance!
    glGetQueryObjectuiv(primitiveQuery,GL_QUERY_RESULT,&active_particles);
//    glGetQueryObjectui64v(timeQuery,GL_QUERY_RESULT,&processtime);

    qDebug("Particle data: %u written",active_particles);

    glUseProgram(shader->getProgramId());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture->getHandle());



    glBindVertexArray(0);
    glUseProgram(0);

    vaoIndex = vboIndex;
    vboIndex = (vboIndex+1)%2;

    glDisable(GL_RASTERIZER_DISCARD);
}

void CoffeeParticleSystem::updateParticles(float delta)
{
    if(!isBaked())
        load();
}

void CoffeeParticleSystem::unload()
{
    texture->unloadTexture();
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();

    shader->buildProgram();

    tshader->createProgram();

    tshader->setVertexShader("ubw/shaders/particles/vertex-process.vert");
    tshader->setGeometryShader("ubw/shaders/particles/geometry-process.geom");

    tshader->compileShaders();

    const GLchar* feedbackattributes[] = {"outType","outPos","outVel","outLife"};
    glTransformFeedbackVaryings(tshader->getProgramId(),4,feedbackattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glUseProgram(tshader->getProgramId());

    tshader->getUniformLocation("mass");
    tshader->getUniformLocation("spawncount");
    tshader->getUniformLocation("timestep");
    tshader->getUniformLocation("gravity");
    tshader->setUniform("mass",0.2f);
    tshader->setUniform("gravity",glm::vec3(0,-9.81,0));

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);

    glBindVertexArray(vaos[vaoIndex]);

    Particle parts[max_particles];

    parts[0].type = 0;
    parts[0].pos = glm::vec3(9,2,9);
    parts[0].vel = glm::vec3(1,5,1);
    parts[0].lifetime = 10.f;

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vaoIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*max_particles, parts, GL_STATIC_DRAW);

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

    glGenQueries(1,&timeQuery);
    glGenQueries(1,&primitiveQuery);

    setBaked(true);
}

void CoffeeParticleSystem::tick(float delta)
{
    updateParticles(delta);
}

bool CoffeeParticleSystem::isBaked()
{
    return baked;
}

void CoffeeParticleSystem::setBaked(bool val)
{
    this->baked = val;
}
