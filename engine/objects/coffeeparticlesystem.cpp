#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeObject(parent)
{
    this->camera = camera;
    tshader = new ShaderContainer(this);
    tshader->setFragmentShader("ubw/shaders/particles/ps_update.frag");
    tshader->setVertexShader("ubw/shaders/particles/ps_update.vert");
    tshader->setGeometryShader("ubw/shaders/particles/ps_update.geom");

    shader = new ShaderContainer(this);
    shader->setFragmentShader("ubw/shaders/particles/billboard.fs");
    shader->setVertexShader("ubw/shaders/particles/billboard.vs");
    shader->setGeometryShader("ubw/shaders/particles/billboard.gs");

    texture = new CoffeeTexture(this,"ubw/models/textures/quadtex.png");
    texture->setObjectName("sprite");

    randTexture = new CoffeeTexture(this,"ubw/models/textures/quadtex.png");
    randTexture->setObjectName("randtexture");
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    glUseProgram(shader->getProgramId());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture->getHandle());

    shader->setUniform("modelview",camera->getMatrix());
    shader->setUniform("cameraPos",camera->getCameraPos());
    shader->setUniform("diffuseSampler",0);
    shader->setUniform("particleSize",particleSize);

    glBindVertexArray(vaos_r[tfIndex]);

//    glBindBuffer(GL_ARRAY_BUFFER,vbos[tfIndex]);

    glDrawTransformFeedback(GL_POINTS,tfbs[tfIndex]);

    glDisableVertexAttribArray(0);
    glUseProgram(0);

    vbIndex = tfIndex;
    tfIndex = (tfIndex+1)%2;
}

void CoffeeParticleSystem::updateParticles(float delta)
{
    if(!isBaked())
        load();
    glUseProgram(tshader->getProgramId());

    tshader->setUniform("fullTime",time*1000.0f);
    tshader->setUniform("deltaTime",delta*1000.0f);
    tshader->setUniform("randSampler",0);
    tshader->setUniform("emitterLife",100.0f);
    tshader->setUniform("shellLife",10000.0f);
    tshader->setUniform("shellSecondLife",2500.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,randTexture->getHandle());

    glEnable(GL_RASTERIZER_DISCARD);

//    glBindBuffer(GL_ARRAY_BUFFER,vbos[vbIndex]);
    glBindVertexArray(vaos_t[tfIndex]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[tfIndex]);


    glBeginTransformFeedback(GL_POINTS);

    if(!started){
        glDrawArrays(GL_POINTS,0,1);
        started = true;
    }else{
        glDrawTransformFeedback(GL_POINTS,tfbs[vbIndex]);
    }

    glEndTransformFeedback();

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_RASTERIZER_DISCARD);
}

void CoffeeParticleSystem::unload()
{
    texture->unloadTexture();
}

void CoffeeParticleSystem::load()
{
    texture->loadTexture();
    randTexture->loadTexture();

    shader->buildProgram();

    tshader->createProgram();
    tshader->compileShaders();

    const GLchar* transfattributes[5] = {"typeArr","posArr","velArr","accArr","lifeArr"};

    glTransformFeedbackVaryings(tshader->getProgramId(),5,transfattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    QStringList transfuniforms;
    transfuniforms << "deltaTime" << "fullTime" << "randSampler"
                   << "emitterLife" << "shellLife" << "shellSecondLife";

    tshader->getUniformLocations(transfuniforms);

    QStringList renduniforms;
    renduniforms << "modelview" << "cameraPos" << "diffuseSampler" << "particleSize";

    shader->getUniformLocations(renduniforms);

    if(max_particles<1)
        qFatal("Particle system cannot have 0 particles!");

    Particle particles[max_particles];

    particles[0].type = EmitterParticleType;
    particles[0].pos = position()->getValue();
    particles[0].vel = glm::vec3(0,0.0002f,0);
    particles[0].acc = glm::vec3(0,0,0);
    particles[0].lifetime = 0.0f;

    glGenTransformFeedbacks(2,tfbs);
    glGenBuffers(2,vbos);
    glGenVertexArrays(2,vaos_r);
    glGenVertexArrays(2,vaos_t);

    for(int i=0;i<2;i++){
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,tfbs[i]);
        glBindVertexArray(vaos_t[i]);
        glBindBuffer(GL_ARRAY_BUFFER,vbos[i]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(particles),particles,GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(0,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)4);
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)16);
        glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)28);
        glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)40);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,vbos[i]);

        glBindVertexArray(vaos_r[(i+1)%2]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(GLvoid*)4);

        glBindVertexArray(0);
    }

    setBaked(true);
}

void CoffeeParticleSystem::tick(float delta)
{
    time+=delta;
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
