#include "coffeemesh.h"

CoffeeMesh::CoffeeMesh(QObject *parent) : QObject(parent){}

CoffeeMesh::CoffeeMesh(QPointer<CoffeeMesh> mesh){
    this->vertices = mesh->vertices;
}

QList<QPointer<CoffeeVertex> > CoffeeMesh::copy(){
    QList<QPointer<CoffeeVertex> > copy;
    for(QPointer<CoffeeVertex> vert : vertices){
        copy.append(new CoffeeVertex(vert));
    }
    return copy;
}

NumberBuffer<GLfloat> *CoffeeMesh::getData(){
    NumberBuffer<GLfloat>* data = NumberBuffer<GLfloat>::createArray(getVerticesSize()*CoffeeVertex::VERTEX_COUNT);
    for(QPointer<CoffeeVertex> vert : vertices){
        data->put(vert->position.x);
        data->put(vert->position.y);
        data->put(vert->position.z);
        data->put(vert->texCoord.x);
        data->put(vert->texCoord.y);
        data->put(vert->normal.x);
        data->put(vert->normal.y);
        data->put(vert->normal.z);
        data->put(vert->tangent.x);
        data->put(vert->tangent.y);
        data->put(vert->tangent.z);
    }
    return data;
}

int CoffeeMesh::getVerticesDataSize(){
    return getVerticesSize()*CoffeeVertex::VERTEX_STRIDE;
}

int CoffeeMesh::getVerticesSize(){
    return vertices.size();
}

QList<QPointer<CoffeeVertex> > CoffeeMesh::getVertices(){
    return vertices;
}

void CoffeeMesh::addVertex(QPointer<CoffeeVertex> vert){
    vertices.append(vert);
}

void CoffeeMesh::setVertices(QList<QPointer<CoffeeVertex> > vertices){
    this->vertices.clear();
    this->vertices.append(vertices);
}

void CoffeeMesh::unloadMesh(){
    GLuint v[1];
    v[0] = vaoHandle();
    glDeleteVertexArrays(1,v);
}

GLint CoffeeMesh::vboHandle() const
{
    return m_vboHandle;
}

GLint CoffeeMesh::vaoHandle() const
{
    return m_vaoHandle;
}

bool CoffeeMesh::streamDraw() const
{
    return m_streamDraw;
}

bool CoffeeMesh::depthTest() const
{
    return m_depthTest;
}

bool CoffeeMesh::baked() const
{
    return m_baked;
}

bool CoffeeMesh::drawn() const
{
    return m_drawn;
}

void CoffeeMesh::setVboHandle(GLint arg)
{
    if (m_vboHandle == arg)
        return;

    m_vboHandle = arg;
    emit vboHandleChanged(arg);
}

void CoffeeMesh::setVaoHandle(GLint arg)
{
    if (m_vaoHandle == arg)
        return;

    m_vaoHandle = arg;
    emit vaoHandleChanged(arg);
}

void CoffeeMesh::setStreamDraw(bool arg)
{
    if (m_streamDraw == arg)
        return;

    m_streamDraw = arg;
    emit streamDrawChanged(arg);
}

void CoffeeMesh::setDepthTest(bool arg)
{
    if (m_depthTest == arg)
        return;

    m_depthTest = arg;
    emit depthTestChanged(arg);
}

void CoffeeMesh::setBaked(bool arg)
{
    if (m_baked == arg)
        return;

    m_baked = arg;
    emit bakedChanged(arg);
}

void CoffeeMesh::setDrawn(bool arg)
{
    if (m_drawn == arg)
        return;

    m_drawn = arg;
    emit drawnChanged(arg);
}
