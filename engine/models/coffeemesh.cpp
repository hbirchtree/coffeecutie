#include "coffeemesh.h"

CoffeeMesh::CoffeeMesh(QObject *parent) : QObject(parent){}

CoffeeMesh::CoffeeMesh(QPointer<CoffeeMesh> mesh){
    this->vertices = mesh->vertices;
}

CoffeeMesh::CoffeeMesh(QObject *parent, aiMesh *meshSource, bool* success) : QObject(parent)
{
    for(uint i=0;i<meshSource->mNumFaces;i++){
        aiFace face = meshSource->mFaces[i];
        for(uint j=0;j<face.mNumIndices;j++){
            indices.append(face.mIndices[j]);
        }
    }
    for(uint i=0;i<meshSource->mNumVertices;i++){
        if(meshSource->HasPositions()){ //I have no idea why.
            aiVector3D pos = meshSource->mVertices[i];
            positions.append(glm::vec3(pos.x,pos.y,pos.z));
        }else{
            if(success)
                *success=false;
        }
        if(meshSource->HasNormals()){
            aiVector3D nor = meshSource->mNormals[i];
            normals.append(glm::vec3(nor.x,nor.y,nor.z));
        }else{
            if(success)
                *success=false;
        }
        if(meshSource->HasTangentsAndBitangents()){
            aiVector3D tan = meshSource->mTangents[i];
            tangents.append(glm::vec3(tan.x,tan.y,tan.z));
            aiVector3D bit = meshSource->mBitangents[i];
            bitangents.append(glm::vec3(bit.x,bit.y,bit.z));
        }else{
            if(success)
                *success=false;
        }
        if(meshSource->HasTextureCoords(0)){
            aiVector3D tex = meshSource->mTextureCoords[0][i];
            texcoords.append(glm::vec2(tex.x,1.f-tex.y));
        }
    }

    if(positions.size()>0)
        m_hasPositions = true;
    if(normals.size()>0)
        m_hasNormals = true;
    if(texcoords.size()>0)
        m_hasTextureCoordinates = true;
    if(tangents.size()>0)
        m_hasTangents = true;
    if(bitangents.size()>0)
        m_hasBitangents = true;

    this->setObjectName(meshSource->mName.C_Str());

    if(success)
        *success = true;
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
    return buffers.at(indexBufferIndex());
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
    int vbuff_count = 1;
    int varray_count = 1;

    if(hasPositions()&&usePositions())
        vbuff_count++;
    if(hasNormals()&&useNormals())
        vbuff_count++;
    if(hasTextureCoordinates()&&useTextureCoordinates())
        vbuff_count++;
    if(hasTangents()&&useTangents())
        vbuff_count++;
    if(hasBitangents()&&useBitangents())
        vbuff_count++;

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

    int free_buffer = 0;

    if(usePositions()&&hasPositions()){
        glBindBuffer(GL_ARRAY_BUFFER,buffers[free_buffer++]);
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
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useTextureCoordinates()&&hasTextureCoordinates()){
        glBindBuffer(GL_ARRAY_BUFFER,buffers[free_buffer++]);
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
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useNormals()&&hasNormals()){
        glBindBuffer(GL_ARRAY_BUFFER,buffers[free_buffer++]);
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
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useTangents()&&hasTangents()){
        glBindBuffer(GL_ARRAY_BUFFER,buffers[free_buffer++]);
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
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useBitangents()&&hasBitangents()){
        glBindBuffer(GL_ARRAY_BUFFER,buffers[free_buffer++]);
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
    }

#endif

    setIndexBufferIndex(free_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[free_buffer++]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size()*sizeof(indices[0]),
                 indices.data(),
                 GL_STATIC_DRAW);

    if(free_buffer!=vbuff_count)
        qWarning("Error when allocating VBOs! Buffers were not used!");

    glBindVertexArray(0);

    setBaked(true);
}

void CoffeeMesh::unloadMesh(){
    glDeleteVertexArrays(arrays.size(),arrays.data());
    glDeleteBuffers(buffers.size(),buffers.data());
}

bool CoffeeMesh::baked() const
{
    return m_baked;
}

bool CoffeeMesh::hasPositions() const
{
    return m_hasPositions;
}

bool CoffeeMesh::hasNormals() const
{
    return m_hasNormals;
}

bool CoffeeMesh::hasTextureCoordinates() const
{
    return m_hasTextureCoordinates;
}

bool CoffeeMesh::hasTangents() const
{
    return m_hasTangents;
}

bool CoffeeMesh::hasBitangents() const
{
    return m_hasBitangents;
}

bool CoffeeMesh::usePositions() const
{
    return m_usePositions;
}

bool CoffeeMesh::useNormals() const
{
    return m_useNormals;
}

bool CoffeeMesh::useTextureCoordinates() const
{
    return m_useTextureCoordinates;
}

bool CoffeeMesh::useBitangents() const
{
    return m_useBitangents;
}

bool CoffeeMesh::useTangents() const
{
    return m_useTangents;
}

int CoffeeMesh::indexBufferIndex() const
{
    return m_indexBufferIndex;
}

void CoffeeMesh::setBaked(bool arg)
{
    m_baked = arg;
}

void CoffeeMesh::setUsePositions(bool usePositions)
{
    m_usePositions = usePositions;
}

void CoffeeMesh::setUseNormals(bool useNormals)
{
    m_useNormals = useNormals;
}

void CoffeeMesh::setUseTextureCoordinates(bool useTextureCoordinates)
{
    m_useTextureCoordinates = useTextureCoordinates;
}

void CoffeeMesh::setUseBitangents(bool useBitangents)
{
    m_useBitangents = useBitangents;
}

void CoffeeMesh::setUseTangents(bool useTangents)
{
    m_useTangents = useTangents;
}

void CoffeeMesh::setIndexBufferIndex(int indexBufferIndex)
{
    m_indexBufferIndex = indexBufferIndex;
}
