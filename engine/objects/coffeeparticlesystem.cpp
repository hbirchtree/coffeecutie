#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeSimpleObject(parent)
{
    this->camera = camera;
    pos = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    shader = new ShaderContainer(this);
    tshader = new ShaderContainer(this);
    texture = new CoffeeTexture(this,"ubw/models/textures/quadtex.png");
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

void CoffeeParticleSystem::setupSystem()
{
    shader->buildProgram("ubw/shaders/particles/particles_render.vert","ubw/shaders/particles/particles_render.frag","ubw/shaders/particles/particles_render.geom");

    shader->getUniformLocation("matrices.mProj");
    shader->getUniformLocation("matrices.mView");
    shader->getUniformLocation("vQuad1");
    shader->getUniformLocation("vQuad2");
    shader->getUniformLocation("gSampler");

    tshader->createProgram();

    std::string src = FileHandler::getStringFromFile("ubw/shaders/particles/particles_update.vert").toStdString();
    const char* code = src.c_str();
    tshader->addShader(code,QString(),GL_VERTEX_SHADER);
    src = FileHandler::getStringFromFile("ubw/shaders/particles/particles_update.geom").toStdString();
    code = src.c_str();
    tshader->addShader(code,QString(),GL_GEOMETRY_SHADER);

    const char* vars[6] = {
        "vPositionOut",
        "vVelocityOut",
        "vColorOut",
        "fLifeTimeOut",
        "fSizeOut",
        "iTypeOut"
    };

    glTransformFeedbackVaryings(tshader->getProgramId(),6,vars,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glGenTransformFeedbacks(1,&transformBuffer);
    glGenQueries(1,&transformQuery);

    glGenBuffers(2,partsBuffers);
    glGenVertexArrays(2,partsArrays);

    Particle initPart;
    initPart.type = ParticleGeneratorType;

    for(int i=0;i<2;i++){
        glBindVertexArray(partsArrays[i]);
        glBindBuffer(GL_ARRAY_BUFFER,partsBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(Particle)*parts_max_count,NULL,GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(Particle),&initPart);

        for(int i=0;i<6;i++){
            glEnableVertexAttribArray(i);
        }

        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)(sizeof(GLfloat)*3));
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)(sizeof(GLfloat)*6));
        glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)(sizeof(GLfloat)*9));
        glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(const GLvoid*)(sizeof(GLfloat)*10));
        glVertexAttribPointer(5,1,GL_INT,GL_FALSE,sizeof(Particle),(const GLvoid*)(sizeof(GLfloat)*11));
    }

    tshader->getUniformLocation("fTimePassed");
    tshader->getUniformLocation("vGenPosition");
    tshader->getUniformLocation("vGenVelocityMin");
    tshader->getUniformLocation("vGenVelocityRange");
    tshader->getUniformLocation("vGenColor");
    tshader->getUniformLocation("vGenGravityVector");

    tshader->getUniformLocation("vRandomSeed");

    tshader->getUniformLocation("fGenLifeMin");
    tshader->getUniformLocation("fGenLifeRange");

    tshader->getUniformLocation("fGenSize");
    tshader->getUniformLocation("iNumToGenerate");

    setBaked(true);
}

void CoffeeParticleSystem::render()
{
    if(!isBaked()){
        setupSystem();
        texture->loadTexture();
    }
    updateParticles(0.5f);

    glm::vec3 q1(1,1,0);
    glm::vec3 q2(-1,1,0);
    q1 = glm::cross(glm::normalize(pos->getValue()-camera->getCameraPos()),camera->getCameraUp());
//    if(q1.length()!=0)
//        q1 = glm::normalize(q1);
    q2 = glm::cross(glm::normalize(pos->getValue()-camera->getCameraPos()),q1);
//    if(q2.length()!=0)
//        q2 = glm::normalize(q2);

    glUseProgram(shader->getProgramId());
//    glDepthMask(GL_FALSE);

    shader->setUniform("matrices.mView",camera->getMatrix());
    shader->setUniform("vQuad1",q1);
    shader->setUniform("vQuad2",q2);
    shader->setUniform("gSampler",texture->getHandle());

    glBindVertexArray(partsArrays[curReadBuffer]);
    glDisableVertexAttribArray(1);

    glDrawArrays(GL_POINTS,0,parts_curr_count);

//    glDepthMask(GL_TRUE);
    glUseProgram(0);
}

void CoffeeParticleSystem::setProperties(glm::vec3 sourcePos,
                                         glm::vec3 minVelocity,
                                         glm::vec3 maxVelocity,
                                         glm::vec3 gravity,
                                         glm::vec3 color,
                                         float minLifetime,
                                         float maxLifetime,
                                         float size,
                                         float renewTime,
                                         float particles)
{
    parts_src_pos = sourcePos;
    parts_ivel = minVelocity;
    parts_rvel = maxVelocity-minVelocity;
    parts_gravity = gravity;
    parts_color = color;
    part_size = size;
    part_life_min = minLifetime;
    part_life_range = maxLifetime-minLifetime;
    parts_renewtime = renewTime;
    parts_gen_count = particles;
}

void CoffeeParticleSystem::updateParticles(float timeStep)
{
    glUseProgram(tshader->getProgramId());

    tshader->setUniform("fTimePassed",parts_time);
    tshader->setUniform("vGenPosition",parts_src_pos);
    tshader->setUniform("vGenVelocityMin",parts_ivel);
    tshader->setUniform("vGenVelocityRange",parts_rvel);
    tshader->setUniform("vGenColor",parts_color);
    tshader->setUniform("vGenGravityVector",parts_gravity);

    tshader->setUniform("fGenLifeMin",part_life_min);
    tshader->setUniform("fGenLifeRange",part_life_range);

    tshader->setUniform("fGenSize",part_size);
    tshader->setUniform("iNumToGenerate",0);

    parts_time += timeStep;

    if(parts_time > parts_renewtime){
        parts_time -= parts_renewtime;
        tshader->setUniform("iNumToGenerate",parts_gen_count);
        tshader->setUniform("vRandomSeed",genRandVec3(-10.f,20.f));
    }

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,transformBuffer);

    glBindVertexArray(partsArrays[curReadBuffer]);
    glEnableVertexAttribArray(1);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,partsBuffers[curReadBuffer]);

    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,transformQuery);
    glBeginTransformFeedback(GL_POINTS);

    glDrawArrays(GL_POINTS,0,parts_curr_count);

    glEndTransformFeedback();

    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    glGetQueryObjectiv(transformQuery,GL_QUERY_RESULT,&parts_curr_count);

    curReadBuffer = 1-curReadBuffer;
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
    glDisable(GL_RASTERIZER_DISCARD);
    glUseProgram(0);
}

float CoffeeParticleSystem::genRandF(float base, float range)
{
    return base+range*(float)(qrand()%(RAND_MAX+1))/(float)RAND_MAX;
}

glm::vec3 CoffeeParticleSystem::genRandVec3(float base,float range)
{
    return glm::vec3(genRandF(base,range),genRandF(base,range),genRandF(base,range));
}
