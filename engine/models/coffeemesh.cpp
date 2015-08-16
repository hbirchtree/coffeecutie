#include "coffeemesh.h"

#include "coffeeinstancecontainer.h"
#include "engine/models/coffeevertexarrayobject.h"
#include "opengl/components/coffeebuffer.h"
#include "coffeeskeleton.h"

CoffeeMesh::CoffeeMesh(QObject *parent) : QObject(parent){
    instances = new CoffeeInstanceContainer(this);
    vertexArray = new CoffeeVertexArrayObject(this);
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
        if(meshSource->HasPositions()){
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
    return indexBuffer->handle();
}

GLuint CoffeeMesh::getVertexArrayHandle()
{
    if(m_doReloadMesh){
        unload();
        load();
        m_doReloadMesh = false;
    }
    return vertexArray->handle();
}

QPointer<CoffeeInstanceContainer> CoffeeMesh::getInstances()
{
    return instances;
}

void CoffeeMesh::renderMesh()
{
    if(useInstancing())
        loadModelMatrices(); //TODO : load only the section of the matrix that needs an update

    glBindVertexArray(getVertexArrayHandle());
    if(useInstancing()){
        glDrawElementsInstanced(GL_TRIANGLES,
                                getIndicesCount(),
                                GL_UNSIGNED_INT,0,
                                getInstances()->instanceCount());
    }else{
        glDrawElements(GL_TRIANGLES,
                       getIndicesCount(),
                       GL_UNSIGNED_INT,
                       0);
    }
    glBindVertexArray(0);
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

void CoffeeMesh::load()
{
    if(isAllocated()){
        addAllocation();
        return;
    }

    int vbuff_count = 1;

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

    QVector<GLuint> buffers;

    buffers.resize(vbuff_count);
    glGenBuffers(vbuff_count,buffers.data());

    vertexArray->allocArray();
    vertexArray->bindArray();

    int free_buffer = 0;

    BufferStorageMask defMask = GL_MAP_WRITE_BIT;

    /*
     *  Future improvement:
     *  - Match buffer with vertex attribute pointer
     *
     */

    if(usePositions()&&hasPositions()){
        CoffeeBuffer *pos = new CoffeeBuffer(0,defMask,GL_ARRAY_BUFFER,buffers[free_buffer++]);
        pos->commitData(positions.data(),positions.size()*sizeof(glm::vec3));
        vertexArray->addAttribute(pos,MESH_LOC_POS,
                                  GL_FLOAT,GL_FALSE,
                                  3,0);
        this->buffers.append(pos);
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useTextureCoordinates()&&hasTextureCoordinates()){
        CoffeeBuffer *pos = new CoffeeBuffer(0,defMask,GL_ARRAY_BUFFER,buffers[free_buffer++]);
        pos->commitData(texcoords.data(),texcoords.size()*sizeof(glm::vec2));
        vertexArray->addAttribute(pos,MESH_LOC_TEX,
                                  GL_FLOAT,GL_FALSE,
                                  2,0);
        this->buffers.append(pos);
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useNormals()&&hasNormals()){
        CoffeeBuffer *pos = new CoffeeBuffer(0,defMask,GL_ARRAY_BUFFER,buffers[free_buffer++]);
        pos->commitData(normals.data(),normals.size()*sizeof(glm::vec3));
        vertexArray->addAttribute(pos,MESH_LOC_NOR,
                                  GL_FLOAT,GL_FALSE,
                                  3,0);
        this->buffers.append(pos);
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useTangents()&&hasTangents()){
        CoffeeBuffer *pos = new CoffeeBuffer(0,defMask,GL_ARRAY_BUFFER,buffers[free_buffer++]);
        pos->commitData(tangents.data(),tangents.size()*sizeof(glm::vec3));
        vertexArray->addAttribute(pos,MESH_LOC_TAN,
                                  GL_FLOAT,GL_FALSE,
                                  3,0);
        this->buffers.append(pos);
    }

    if(free_buffer>=vbuff_count)
        qFatal("Error when allocating VBOs! Not enough buffers!");

    if(useBitangents()&&hasBitangents()){
        CoffeeBuffer *pos = new CoffeeBuffer(0,defMask,GL_ARRAY_BUFFER,buffers[free_buffer++]);
        pos->commitData(bitangents.data(),bitangents.size()*sizeof(glm::vec3));
        vertexArray->addAttribute(pos,MESH_LOC_BIT,
                                  GL_FLOAT,GL_FALSE,
                                  3,0);
        this->buffers.append(pos);
    }

    if(useInstancing()){
        matrixBuffer = new
                CoffeeBuffer(0,
                             GL_DYNAMIC_STORAGE_BIT,
                             GL_ARRAY_BUFFER,
                             buffers[free_buffer++]);
        matrixBuffer->commitData(instances->getData().data(),
                                 sizeof(glm::mat4)*instances->instanceCount());
        for(int i=0;i<4;i++){
            vertexArray->addAttributeDivided(matrixBuffer,MESH_LOC_MODEL_MAT+i,
                                      GL_FLOAT,GL_FALSE,
                                      4,sizeof(glm::mat4),1,(GLvoid*)(sizeof(GLfloat)*4*i));
        }
    }

    indexBuffer = new
            CoffeeBuffer(0,
                         GL_MAP_WRITE_BIT,
                         GL_ELEMENT_ARRAY_BUFFER,
                         buffers[free_buffer++]);
    indexBuffer->commitData(indices.data(),sizeof(indices[0])*indices.size());

    if(free_buffer!=vbuff_count)
        qWarning("VBOs were allocated but not used!");

    vertexArray->unbindArray();

    addAllocation();
    setBaked(true);
}

void CoffeeMesh::unload(){
    removeAllocation();
    if(isAllocated())
        return;
    vertexArray->freeArray();
    for(CoffeeBuffer* b : buffers){
        b->freeBuffer();
        delete b;
    }
    if(matrixBuffer)
        matrixBuffer->freeBuffer();
    indexBuffer->freeBuffer();
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
    QVector<glm::mat4> data = instances->getData();
    matrixBuffer->commitSubData(data.data(),0,sizeof(glm::mat4)*instances->instanceCount());
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
