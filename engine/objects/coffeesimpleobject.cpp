#include "coffeesimpleobject.h"

CoffeeSimpleObject::CoffeeSimpleObject(QObject *parent) : QObject(parent)
{

}

void CoffeeSimpleObject::render()
{
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
    Q_UNUSED(shader);
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

