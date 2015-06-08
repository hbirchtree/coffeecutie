#ifndef COFFEEMESH
#define COFFEEMESH

#include "general/common.h"
#include "general/data/coffeegameasset.h"
#include "general/data/numberbuffer.h"

class CoffeeInstanceContainer;

#define MESH_LOC_POS 0
#define MESH_LOC_TEX 1
#define MESH_LOC_NOR 2
#define MESH_LOC_TAN 3
#define MESH_LOC_BIT 4
#define MESH_LOC_MODEL_MAT 5

class CoffeeMesh : public QObject, public CoffeeGameAsset{
    Q_OBJECT

    Q_PROPERTY(bool hasPositions READ hasPositions)
    Q_PROPERTY(bool hasNormals READ hasNormals)
    Q_PROPERTY(bool hasTextureCoordinates READ hasTextureCoordinates)
    Q_PROPERTY(bool hasTangents READ hasTangents)
    Q_PROPERTY(bool hasBitangents READ hasBitangents)

    Q_PROPERTY(bool usePositions READ usePositions WRITE setUsePositions)
    Q_PROPERTY(bool useNormals READ useNormals WRITE setUseNormals)
    Q_PROPERTY(bool useTextureCoordinates READ useTextureCoordinates WRITE setUseTextureCoordinates)
    Q_PROPERTY(bool useTangents READ useTangents WRITE setUseTangents)
    Q_PROPERTY(bool useBitangents READ useBitangents WRITE setUseBitangents)

    Q_PROPERTY(bool useInstancing READ useInstancing WRITE setUseInstancing)

    Q_PROPERTY(bool baked READ baked WRITE setBaked)

    Q_PROPERTY(int indexBufferIndex READ indexBufferIndex WRITE setIndexBufferIndex)

public:
    class VertexArrayPointerDescriptor {
    public:
        int location = 0;
        int size = 0;
        int datasize = 0;
        GLenum datatype = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
    };

    CoffeeMesh(QObject* parent);
    CoffeeMesh(QObject* parent, aiMesh* meshSource, bool *success = 0);

    GLuint getVertexIndexHandle() const;
    GLuint getVertexArrayHandle() const;

    GLuint getIndicesCount() const;

    void loadMesh();
    void unloadMesh();

    bool baked() const;

    bool hasPositions() const;
    bool hasNormals() const;
    bool hasTextureCoordinates() const;
    bool hasTangents() const;
    bool hasBitangents() const;

    bool usePositions() const;
    bool useNormals() const;
    bool useTextureCoordinates() const;
    bool useBitangents() const;
    bool useTangents() const;

    int indexBufferIndex() const;

    bool useInstancing() const;
    bool hasNewMatrices() const;

    QPointer<CoffeeInstanceContainer> getInstances();

public slots:
    void updateModelMatrices();
    void loadModelMatrices();

    void setBaked(bool arg);

    void setUsePositions(bool usePositions);
    void setUseNormals(bool useNormals);
    void setUseTextureCoordinates(bool useTextureCoordinates);
    void setUseBitangents(bool useBitangents);
    void setUseTangents(bool useTangents);

    void setIndexBufferIndex(int indexBufferIndex);

    void setUseInstancing(bool useInstancing);

protected:
    void generateIndices();

    QVector<glm::vec3> positions;
    QVector<glm::vec2> texcoords;
    QVector<glm::vec3> normals;
    QVector<glm::vec3> bitangents;
    QVector<glm::vec3> tangents;
    QVector<GLuint> indices;

    NumberBuffer<GLfloat>* getData();
    int getVerticesDataSize();


private:
    uint matrixbuffer = 0;

    QPointer<CoffeeInstanceContainer> instances;

    QVector<GLuint> buffers;
    QVector<GLuint> arrays;
    GLenum drawmode = GL_STATIC_DRAW;
    bool m_baked = false;

    bool m_hasPositions = false;
    bool m_hasNormals = false;
    bool m_hasTextureCoordinates = false;
    bool m_hasTangents = false;
    bool m_hasBitangents = false;
    bool m_usePositions = true;
    bool m_useNormals = true;
    bool m_useTextureCoordinates = true;
    bool m_useBitangents = false;
    bool m_useTangents = true;
    int m_indexBufferIndex;

    bool m_newMatrices = false;
    bool m_useInstancing = false;
};

#endif // COFFEEMESH

