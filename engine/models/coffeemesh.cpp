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

GLuint CoffeeMesh::getIndicesCount() const
{
    return indices.size();
}

void CoffeeMesh::generateIndices()
{
    for(CoffeeVertex* vert : vertices){
        vert->i_tangent = raw_tangents.size();
        raw_tangents.append(vert->tangent);
        indices.append(vert->i_position);
        indices.append(vert->i_tex);
        indices.append(vert->i_normal);
        indices.append(vert->i_tangent);
    }
}

void CoffeeMesh::loadMesh()
{
//    QVector<VertexArrayPointerDescriptor> pts;
//    VertexArrayPointerDescriptor pos;
//    pos.location = 0;
//    pos.datatype = GL_FLOAT;
//    pos.size = 3;
//    pos.datasize = sizeof(GLfloat);
//    VertexArrayPointerDescriptor nor;
//    nor.location = 1;
//    nor.datatype = GL_FLOAT;
//    nor.size = 3;
//    nor.datasize = sizeof(GLfloat);
//    VertexArrayPointerDescriptor tex;
//    tex.location = 2;
//    tex.datatype = GL_FLOAT;
//    tex.size = 2;
//    tex.datasize = sizeof(GLfloat);
//    VertexArrayPointerDescriptor tng;
//    tng.location = 3;
//    tng.datatype = GL_FLOAT;
//    tng.size = 3;
//    tng.datasize = sizeof(GLfloat);

//    pts << pos << nor << tex << tng;

//    int stride = 0;
//    for(VertexArrayPointerDescriptor t : pts)
//        stride+=t.size*t.datasize;

    int vbuff_count = 5;
    int varray_count = 1;

    buffers.resize(vbuff_count);
    arrays.resize(varray_count);
    glGenBuffers(vbuff_count,buffers.data());
    glGenVertexArrays(varray_count,arrays.data());

    generateIndices();

    glBindVertexArray(arrays.at(0));

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_POS]);
    glBufferData(GL_ARRAY_BUFFER,raw_vertices.size()*sizeof(GLfloat),
                 &raw_vertices.data()[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_POS);
    glVertexAttribPointer(MESH_LOC_POS,3,
                          GL_FLOAT,GL_FALSE,
                          0,(GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TEX]);
    glBufferData(GL_ARRAY_BUFFER,raw_texcoords.size()*sizeof(GLfloat),
                 &raw_texcoords.data()[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TEX);
    glVertexAttribPointer(MESH_LOC_TEX,2,
                          GL_FLOAT,GL_TRUE,
                          0,(GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER,raw_normals.size()*sizeof(GLfloat),
                 &raw_normals.data()[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_NOR);
    glVertexAttribPointer(MESH_LOC_NOR,3,
                          GL_FLOAT,GL_FALSE,
                          0,(GLvoid*)0);

    //TODO : write tangents to raw_tangents instead of each individual vertex. vertices will now only hold indices. transitioning is hard but necessary.
    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TANGENT]);
    glBufferData(GL_ARRAY_BUFFER,raw_tangents.size()*sizeof(GLfloat),
                 &raw_tangents.data()[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TAN);
    glVertexAttribPointer(MESH_LOC_TAN,3,
                          GL_FLOAT,GL_FALSE,
                          0,(GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[MESH_BUFFER_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(indices[0]),
                 &indices.data()[0],GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER,buffers.at(0));
//    NumberBuffer<GLfloat> *bufferdata = getData();
//    glBufferData(GL_ARRAY_BUFFER,bufferdata->getSize()*sizeof(GLfloat),bufferdata->get(),drawmode);


//    int accumulated_stride = 0;
//    for(VertexArrayPointerDescriptor t : pts){
//        glEnableVertexAttribArray(t.location);
//        glVertexAttribPointer(t.location,t.size,t.datatype,t.normalized,
//                              stride,(GLvoid*)accumulated_stride);
//        accumulated_stride += t.size*t.datasize;
//    }

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
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
