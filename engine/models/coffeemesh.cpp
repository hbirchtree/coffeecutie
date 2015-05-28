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
    NumberBuffer<GLfloat>* data =
            NumberBuffer<GLfloat>::createArray(this,getVerticesSize()*CoffeeVertex::VERTEX_COUNT);
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

void CoffeeMesh::loadMesh()
{
    buffers.reserve(7);
    arrays.reserve(1);
    glGenBuffers(7,buffers.data());
    glGenVertexArrays(1,arrays.data());

}

void CoffeeMesh::unloadMesh(){
    glDeleteVertexArrays(1,vao.data());
}

bool CoffeeMesh::baked() const
{
    return m_baked;
}

void CoffeeMesh::setBaked(bool arg)
{
    if (m_baked == arg)
        return;

    m_baked = arg;
}
