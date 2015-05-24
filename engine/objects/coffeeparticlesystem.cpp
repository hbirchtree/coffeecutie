#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeSimpleObject(parent)
{
    this->camera = camera;
    pos = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    shader = new ShaderContainer(this);
    tshader = new ShaderContainer(this);
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

    tshader->getUniformLocation("fGenLifeMin");
    tshader->getUniformLocation("fGenLifeRange");

    tshader->getUniformLocation("fGenSize");
    tshader->getUniformLocation("iNumToGenerate");

    setBaked(true);
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        setupSystem();
}

void CoffeeParticleSystem::updateParticles(float timeStep)
{
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
    }
}
