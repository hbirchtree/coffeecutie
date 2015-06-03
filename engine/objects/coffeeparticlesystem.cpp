#include "coffeeparticlesystem.h"

CoffeeParticleSystem::CoffeeParticleSystem(QObject *parent,const CoffeeCamera* camera) : CoffeeObject(parent)
{
    this->camera = camera;


    tshader = new ShaderContainer(this);


    texture = new CoffeeTexture(this,"ubw/models/textures/quadtex.png");
    texture->setObjectName("sprite");
}

void CoffeeParticleSystem::render()
{
    if(!isBaked())
        load();

    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(tshader->getProgramId());
    glBindVertexArray(vao);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,tbo);
    glBeginTransformFeedback(GL_POINTS);

    glDrawArrays(GL_POINTS,0,5);

    glEndTransformFeedback();

    glFlush();

    GLfloat feedback[6];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER,0,sizeof(feedback),feedback);

    qDebug() << feedback[0] << feedback[1] << feedback[2] << feedback[3] << feedback[4] << feedback[5];

    glBindVertexArray(0);
    glUseProgram(0);

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

    tshader->createProgram();

    tshader->setVertexShader("ubw/shaders/particles/vertex-process.vert");
    tshader->setGeometryShader("ubw/shaders/particles/geometry-process.geom");

    tshader->compileShaders();

    const GLchar* feedbackattributes[] = {"outValue"};
    glTransformFeedbackVaryings(tshader->getProgramId(),1,feedbackattributes,GL_INTERLEAVED_ATTRIBS);

    tshader->linkProgram();

    glUseProgram(tshader->getProgramId());

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat data[] = { 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    GLint inAttr = tshader->getAttributeLocation("inValue");
    glEnableVertexAttribArray(inAttr);
    glVertexAttribPointer(inAttr,3,GL_FLOAT,GL_FALSE,0,0);

    glGenBuffers(1,&tbo);
    glBindBuffer(GL_ARRAY_BUFFER,tbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(data),nullptr,GL_STATIC_READ);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

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
