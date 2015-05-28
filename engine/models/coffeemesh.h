#ifndef COFFEEMESH
#define COFFEEMESH

#include "general/common.h"
#include "opengl/rendering/coffeevertex.h"
#include "general/data/coffeegameasset.h"
#include "general/data/numberbuffer.h"
class CoffeeMesh : public QObject, public CoffeeGameAsset{
    Q_OBJECT

    Q_PROPERTY(int vaoHandle READ vaoHandle)
    Q_PROPERTY(int vboHandle READ vboHandle)
    Q_PROPERTY(bool streamDraw READ streamDraw)
    Q_PROPERTY(bool baked READ baked WRITE setBaked)

public:
    struct VertexArrayPointerDescriptor {
        int location = 0;
        int size = 0;
        GLenum datatype = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
    };

    CoffeeMesh(QObject* parent);
    CoffeeMesh(QPointer<CoffeeMesh> mesh);

    QList<QPointer<CoffeeVertex> > copy();

    QVector<glm::vec3> raw_vertices;
    QVector<glm::vec3> raw_normals;
    QVector<glm::vec2> raw_texcoords;
    QVector<int> raw_faces;

    QList<QPointer<CoffeeVertex> > getVertices();
    void addVertex(QPointer<CoffeeVertex> vert);
    void setVertices(QList<QPointer<CoffeeVertex> > vertices);

    void loadMesh();
    void unloadMesh();

    bool baked() const;
public slots:
    void setBaked(bool arg);

private:

    NumberBuffer<GLfloat>* getData();
    int getVerticesDataSize();
    int getVerticesSize();

    QList<QPointer<CoffeeVertex> > vertices;
    std::vector<GLuint> buffers;
    std::vector<GLuint> arrays;
    GLenum drawmode = GL_STATIC_DRAW;
    bool m_baked = false;
};

#endif // COFFEEMESH

