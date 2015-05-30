#include "coffeemesh.h"

CoffeeMesh::CoffeeMesh(QObject *parent) : QObject(parent){}

CoffeeMesh::CoffeeMesh(QPointer<CoffeeMesh> mesh){
    this->vertices = mesh->vertices;
}

CoffeeMesh::CoffeeMesh(QObject *parent, aiMesh *meshSource) : QObject(parent)
{
    for(int i=0;i<meshSource->mNumFaces;i++){
        aiFace face = meshSource->mFaces[i];
        for(int j=0;j<face.mNumIndices;j++){
            indices.append(face.mIndices[j]);
        }
    }
    for(int i=0;i<meshSource->mNumVertices;i++){
        aiVector3D pos = meshSource->mVertices[i];
        aiVector3D nor = meshSource->mNormals[i];
        aiVector3D tan = meshSource->mTangents[i];
        aiVector3D bit = meshSource->mBitangents[i];

        positions.append(glm::vec3(pos.x,pos.y,pos.z));
        normals.append(glm::vec3(nor.x,nor.y,nor.z));
        tangents.append(glm::vec3(tan.x,tan.y,tan.z));
        bitangents.append(glm::vec3(bit.x,bit.y,bit.z));
        texcoords.append(glm::vec2(0,0));
    }
    this->setObjectName(meshSource->mName.C_Str());
}

QList<QPointer<CoffeeVertex> > CoffeeMesh::copy(){
    QList<QPointer<CoffeeVertex> > copy;
    for(QPointer<CoffeeVertex> vert : vertices){
        copy.append(new CoffeeVertex(vert));
    }
    return copy;
}

GLuint CoffeeMesh::getVertexIndexHandle() const
{
    return buffers.at(MESH_BUFFER_INDEX);
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
    int i=0;
    while(i+2<vertices.size()){
        CoffeeVertex* v1 = vertices.at(i);
        CoffeeVertex* v2 = vertices.at(i+1);
        CoffeeVertex* v3 = vertices.at(i+2);

        indices.append(v1->i_position);
        indices.append(v2->i_position);
        indices.append(v3->i_position);

        v1->i_tangent = tangents.size();
        tangents.append(v1->tangent);
        v2->i_tangent = tangents.size();
        tangents.append(v2->tangent);
        v3->i_tangent = tangents.size();
        tangents.append(v3->tangent);

        i+=3;
    }
}

void CoffeeMesh::loadMesh()
{
    int vbuff_count = 5;
    int varray_count = 1;

    buffers.resize(vbuff_count);
    arrays.resize(varray_count);
    glGenBuffers(vbuff_count,buffers.data());
    glGenVertexArrays(varray_count,arrays.data());

    glBindVertexArray(arrays.at(0));

#ifdef MESH_INDEXED_RAW
    generateIndices();

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_POS]);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_vertices.size()*sizeof(glm::vec3),
                 raw_vertices.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_POS);
    glVertexAttribPointer(MESH_LOC_POS,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TEX]);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_texcoords.size()*sizeof(glm::vec2),
                 raw_texcoords.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TEX);
    glVertexAttribPointer(MESH_LOC_TEX,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_normals.size()*sizeof(glm::vec3),
                 raw_normals.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_NOR);
    glVertexAttribPointer(MESH_LOC_NOR,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);

    //TODO : write tangents to raw_tangents instead of each individual vertex. vertices will now only hold indices. transitioning is hard but necessary.
    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TANGENT]);
    glBufferData(GL_ARRAY_BUFFER,
                 raw_tangents.size()*sizeof(glm::vec3),
                 raw_tangents.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TAN);
    glVertexAttribPointer(MESH_LOC_TAN,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);
#else

#ifdef COFFEE_USE_HORRIBLE_OBJ_IMPORTER
    QVector<glm::vec3> positions;
    QVector<glm::vec2> texcoords;
    QVector<glm::vec3> normals;
    QVector<glm::vec3> tangents;

    for(GLuint i=0;i<vertices.size();i++){
        CoffeeVertex* vert = vertices.at(i);
        positions.append(vert->position);
        texcoords.append(vert->texCoord);
        normals.append(vert->normal);
        tangents.append(vert->tangent);
        indices.append(i);
    }
#endif

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_POS]);
    glBufferData(GL_ARRAY_BUFFER,
                 positions.size()*sizeof(glm::vec3),
                 positions.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_POS);
    glVertexAttribPointer(MESH_LOC_POS,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TEX]);
    glBufferData(GL_ARRAY_BUFFER,
                 texcoords.size()*sizeof(glm::vec2),
                 texcoords.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TEX);
    glVertexAttribPointer(MESH_LOC_TEX,
                          2,
                          GL_FLOAT,
                          GL_TRUE,
                          0,
                          (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_NORMAL]);
    glBufferData(GL_ARRAY_BUFFER,
                 normals.size()*sizeof(glm::vec3),
                 normals.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_NOR);
    glVertexAttribPointer(MESH_LOC_NOR,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER,buffers[MESH_BUFFER_TANGENT]);
    glBufferData(GL_ARRAY_BUFFER,
                 tangents.size()*sizeof(glm::vec3),
                 tangents.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_LOC_TAN);
    glVertexAttribPointer(MESH_LOC_TAN,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (GLvoid*)0);
#endif

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[MESH_BUFFER_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size()*sizeof(indices[0]),
                 indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void CoffeeMesh::unloadMesh(){
    glDeleteVertexArrays(arrays.size(),arrays.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[MESH_BUFFER_INDEX]);
    glDeleteBuffers(buffers.size(),buffers.data());
}

bool CoffeeMesh::baked() const
{
    return m_baked;
}

void CoffeeMesh::setBaked(bool arg)
{
    m_baked = arg;
}
