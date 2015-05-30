#ifndef COFFEEMESH
#define COFFEEMESH

#include "general/common.h"
#include "opengl/rendering/coffeevertex.h"
#include "general/data/coffeegameasset.h"
#include "general/data/numberbuffer.h"

#define MESH_BUFFER_INDEX 4
#define MESH_BUFFER_POS 0
#define MESH_BUFFER_TEX 1
#define MESH_BUFFER_NORMAL 2
#define MESH_BUFFER_TANGENT 3

#define MESH_BUFFER_WVP_MAT 5
#define MESH_BUFFER_WORLD_MAT 6

#define MESH_LOC_POS 0
#define MESH_LOC_TEX 1
#define MESH_LOC_NOR 2
#define MESH_LOC_TAN 3
#define MESH_LOC_WVP_MAT 4
#define MESH_LOC_WLD_MAT 7

//#define MESH_INDEXED_RAW

class CoffeeMesh : public QObject, public CoffeeGameAsset{
    Q_OBJECT

    Q_PROPERTY(bool baked READ baked WRITE setBaked)

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
    CoffeeMesh(QPointer<CoffeeMesh> mesh);

    QList<QPointer<CoffeeVertex> > copy();

    QVector<glm::vec3> raw_vertices;
    QVector<glm::vec2> raw_texcoords;
    QVector<glm::vec3> raw_normals;
    QVector<glm::vec3> raw_tangents;
    QVector<GLuint> indices;

    GLuint getVertexIndexHandle() const;
    GLuint getVertexArrayHandle() const;

    int getVerticesSize();
    void addVertex(QPointer<CoffeeVertex> vert);
    void setVertices(QList<QPointer<CoffeeVertex> > vertices);

    GLuint getIndicesCount() const;
    void generateIndices();

    void loadMesh();
    void unloadMesh();

    bool baked() const;
public slots:
    void setBaked(bool arg);

protected:
    QList<QPointer<CoffeeVertex> > getVertices();
    NumberBuffer<GLfloat>* getData();
    int getVerticesDataSize();

    QList<QPointer<CoffeeVertex> > vertices;
    QVector<GLuint> buffers;
    QVector<GLuint> arrays;
    GLenum drawmode = GL_STATIC_DRAW;
    bool m_baked = false;
};

#endif // COFFEEMESH

