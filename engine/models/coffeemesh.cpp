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

GLuint CoffeeMesh::getVertexArrayHandle() const
{
    return arrays.at(0);
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
    QVector<VertexArrayPointerDescriptor> pts;
    VertexArrayPointerDescriptor pos;
    pos.location = 0;
    pos.datatype = GL_FLOAT;
    pos.size = 3;
    pos.datasize = sizeof(GLfloat);
    VertexArrayPointerDescriptor nor;
    nor.location = 1;
    nor.datatype = GL_FLOAT;
    nor.size = 3;
    nor.datasize = sizeof(GLfloat);
    VertexArrayPointerDescriptor tex;
    tex.location = 2;
    tex.datatype = GL_FLOAT;
    tex.size = 2;
    tex.datasize = sizeof(GLfloat);
    VertexArrayPointerDescriptor tng;
    tng.location = 3;
    tng.datatype = GL_FLOAT;
    tng.size = 3;
    tng.datasize = sizeof(GLfloat);

    pts << pos << nor << tex << tng;

    int stride = 0;
    for(VertexArrayPointerDescriptor t : pts)
        stride+=t.size*t.datasize;

    int vbuff_count = 1;
    int varray_count = 1;

    buffers.resize(vbuff_count);
    arrays.resize(varray_count);
    glGenBuffers(vbuff_count,buffers.data());
    glGenVertexArrays(varray_count,arrays.data());

    glBindBuffer(GL_ARRAY_BUFFER,buffers.at(0));
    NumberBuffer<GLfloat> *bufferdata = getData();
    glBufferData(GL_ARRAY_BUFFER,bufferdata->getSize()*sizeof(GLfloat),bufferdata->get(),drawmode);

    glBindVertexArray(arrays.at(0));

    int accumulated_stride = 0;
    for(VertexArrayPointerDescriptor t : pts){
        glEnableVertexAttribArray(t.location);
        glVertexAttribPointer(t.location,t.size,t.datatype,t.normalized,
                              stride,(GLvoid*)accumulated_stride);
        accumulated_stride += t.size*t.datasize;
    }

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void CoffeeMesh::unloadMesh(){
    glDeleteVertexArrays(arrays.size(),arrays.data());
    glDeleteBuffers(buffers.size(),buffers.data());
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
