#include "coffeesimpleobject.h"

CoffeeSimpleObject::CoffeeSimpleObject(QObject *parent,CoffeeFrameBufferObject* display) : QObject(parent)
{
    mdl = NumberBuffer<GLfloat>::createArray(this,18);
    GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    shader = new ShaderContainer(this);
    shader->buildProgram("testgame/shaders/vsh_passthrough.txt","testgame/shaders/fsh_simple.txt");
    mdl->put(g_quad_vertex_buffer_data);
    texture = display->getTextureHandle();

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(g_quad_vertex_buffer_data),g_quad_vertex_buffer_data,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(shader->getAttributeLocation("vert"));
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shader->getUniformLocation("screen");
    shader->getUniformLocation("time");
}

void CoffeeSimpleObject::render()
{
    glUseProgram(shader->getProgramId());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    shader->setUniform("screen",0);
    shader->setUniform("time",(float)glfwGetTime());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glUseProgram(0);
}

glm::vec3 CoffeeSimpleObject::getPosition() const
{
    return pos;
}

glm::quat CoffeeSimpleObject::getRotation() const
{
    return glm::quat(1,0,0,0);
}

glm::vec3 CoffeeSimpleObject::getScale() const
{
    return scale;
}

QPointer<CoffeeMaterial> CoffeeSimpleObject::getMaterial()
{
    return mtl;
}

GLint CoffeeSimpleObject::getVaoHandle()
{
    return vao;
}

GLint CoffeeSimpleObject::getVboHandle()
{
    return vbo;
}

void CoffeeSimpleObject::setVaoHandle(GLint handle)
{
    vao = handle;
}

void CoffeeSimpleObject::setVboHandle(GLint handle)
{
    vbo = handle;
}

int CoffeeSimpleObject::getVertexDataSize()
{
    return mdl->getSize()*sizeof(GLfloat);
}

int CoffeeSimpleObject::getVerticesCount()
{
    return mdl->getSize();
}

NumberBuffer<GLfloat> *CoffeeSimpleObject::getVertexData()
{
    return mdl;
}

bool CoffeeSimpleObject::isStreamDraw()
{
    return false;
}

bool CoffeeSimpleObject::isDepthTest()
{
    return true;
}

bool CoffeeSimpleObject::isDrawn()
{
    return true;
}

bool CoffeeSimpleObject::isBaked()
{
    return baked;
}

void CoffeeSimpleObject::setBaked(bool val)
{
    baked = val;
}

QPointer<ShaderContainer> CoffeeSimpleObject::getShader()
{
    return shader;
}

void CoffeeSimpleObject::setShader(QPointer<ShaderContainer> shader)
{

}

void CoffeeSimpleObject::unloadAssets()
{
    GLuint v[1];
    v[0] = getVaoHandle();
    glDeleteVertexArrays(1,v);
    mtl->unloadData();
}

void CoffeeSimpleObject::setMdl(QPointer<NumberBuffer<GLfloat> > value)
{
    mdl = value;
}

