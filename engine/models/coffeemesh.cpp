#include "coffeemesh.h"

#include "coffeeinstancecontainer.h"
#include "coffeeskeleton.h"

CoffeeMesh::CoffeeMesh(QObject *parent) : QObject(parent){
    instances = new CoffeeInstanceContainer(this);
    instances->setObjectName("instances");
}

CoffeeMesh::CoffeeMesh(QObject *parent, aiMesh *meshSource, aiNode* rootNode, bool* success) : CoffeeMesh(parent)
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
    if(meshSource->HasBones()){
        try{
             skeleton = new CoffeeSkeleton(this,meshSource->mBones,meshSource->mNumBones,rootNode);
        }catch(CoffeeSkeletonException e){
            qDebug("Failed to create skeleton: Error %i",e.type());
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

GLuint CoffeeMesh::getVertexIndexHandle() const
{
    return buffers.at(indexBufferIndex());
}

GLuint CoffeeMesh::getVertexArrayHandle()
{
    if(m_doReloadMesh){
        unloadMesh();
        loadMesh();
        m_doReloadMesh = false;
    }
    return arrays.at(0);
}

QPointer<CoffeeInstanceContainer> CoffeeMesh::getInstances()
{
    return instances;
}

bool CoffeeMesh::hasSkeleton() const
{
    return m_hasSkeleton;
}

bool CoffeeMesh::useSkeleton() const
{
    return m_useSkeleton;
}

GLuint CoffeeMesh::getIndicesCount() const
{
    return indices.size();
}

void CoffeeMesh::loadMesh()
{
    if(isAllocated()){
        addAllocation();
        return;
    }

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
    if(useInstancing())
        vbuff_count++;

    buffers.resize(vbuff_count);
    arrays.resize(varray_count);
    glGenBuffers(vbuff_count,buffers.data());
    glGenVertexArrays(varray_count,arrays.data());

    glBindVertexArray(arrays.at(0));

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

    if(useInstancing()){
        uint instance_buffer = free_buffer++;
        glBindBuffer(GL_ARRAY_BUFFER,buffers[instance_buffer]);
        for(int i=0;i<4;i++){
            glEnableVertexAttribArray(MESH_LOC_MODEL_MAT+i);
            glVertexAttribPointer(MESH_LOC_MODEL_MAT+i,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(glm::mat4),
                                  (GLvoid*)(sizeof(GLfloat)*4*i));
            glVertexAttribDivisor(MESH_LOC_MODEL_MAT+i,1);
        }
        matrixbuffer = instance_buffer;
        loadModelMatrices();
    }

    setIndexBufferIndex(free_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[free_buffer++]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size()*sizeof(indices[0]),
                 indices.data(),
                 GL_STATIC_DRAW);

    if(free_buffer!=vbuff_count)
        qWarning("Error when allocating VBOs! Buffers were not used!");

    glBindVertexArray(0);

    addAllocation();
    setBaked(true);
}

void CoffeeMesh::unloadMesh(){
    removeAllocation();
    if(isAllocated())
        return;
    matrixbuffer = 0;
    m_indexBufferIndex = 0;
    glDeleteVertexArrays(arrays.size(),arrays.data());
    glDeleteBuffers(buffers.size(),buffers.data());
    arrays.clear();
    buffers.clear();
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

bool CoffeeMesh::useInstancing() const
{
    return m_useInstancing;
}

bool CoffeeMesh::hasNewMatrices() const
{
    return m_newMatrices;
}

QObject *CoffeeMesh::getInstancesQObject()
{
    return getInstances();
}

void CoffeeMesh::updateModelMatrices()
{
    m_newMatrices = true;
}

void CoffeeMesh::loadModelMatrices()
{
    if(matrixbuffer==0)
        return;
    glBindBuffer(GL_ARRAY_BUFFER,buffers[matrixbuffer]);
    QVector<glm::mat4> data = instances->getData();
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::mat4)*instances->instanceCount(),
                 data.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
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

void CoffeeMesh::setUseInstancing(bool useInstancing)
{
    m_useInstancing = useInstancing;
    m_doReloadMesh = true;
}

void CoffeeMesh::setUseSkeleton(bool useSkeleton)
{
    m_useSkeleton = useSkeleton;
}
