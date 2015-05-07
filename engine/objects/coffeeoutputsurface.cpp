#include "coffeeoutputsurface.h"

CoffeeOutputSurface::CoffeeOutputSurface(QObject *parent,CoffeeFrameBufferObject* display) : CoffeeSimpleObject(parent)
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

void CoffeeOutputSurface::render()
{
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shader->getProgramId());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    shader->setUniform("screen",0);
    shader->setUniform("time",(float)glfwGetTime());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}

glm::vec3 CoffeeOutputSurface::getPosition() const
{
    return pos;
}

glm::quat CoffeeOutputSurface::getRotation() const
{
    return glm::quat(1,0,0,0);
}

glm::vec3 CoffeeOutputSurface::getScale() const
{
    return scale;
}

QPointer<CoffeeMaterial> CoffeeOutputSurface::getMaterial()
{
    return mtl;
}

GLint CoffeeOutputSurface::getVaoHandle()
{
    return vao;
}

GLint CoffeeOutputSurface::getVboHandle()
{
    return vbo;
}

void CoffeeOutputSurface::setVaoHandle(GLint handle)
{
    vao = handle;
}

void CoffeeOutputSurface::setVboHandle(GLint handle)
{
    vbo = handle;
}

int CoffeeOutputSurface::getVertexDataSize()
{
    return mdl->getSize()*sizeof(GLfloat);
}

int CoffeeOutputSurface::getVerticesCount()
{
    return mdl->getSize();
}

NumberBuffer<GLfloat> *CoffeeOutputSurface::getVertexData()
{
    return mdl;
}

bool CoffeeOutputSurface::isStreamDraw()
{
    return false;
}

bool CoffeeOutputSurface::isDepthTest()
{
    return true;
}

bool CoffeeOutputSurface::isDrawn()
{
    return true;
}

bool CoffeeOutputSurface::isBaked()
{
    return baked;
}

void CoffeeOutputSurface::setBaked(bool val)
{
    baked = val;
}

QPointer<ShaderContainer> CoffeeOutputSurface::getShader()
{
    return shader;
}

void CoffeeOutputSurface::setShader(QPointer<ShaderContainer> shader)
{
    Q_UNUSED(shader);
}

void CoffeeOutputSurface::unloadAssets()
{
    GLuint v[1];
    v[0] = getVaoHandle();
    glDeleteVertexArrays(1,v);
    mtl->unloadData();
}

void CoffeeOutputSurface::setMdl(QPointer<NumberBuffer<GLfloat> > value)
{
    mdl = value;
}

