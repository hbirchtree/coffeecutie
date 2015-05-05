#ifndef COFFEEMESH
#define COFFEEMESH

#include "general/common.h"
#include "opengl/rendering/coffeevertex.h"
#include "general/data/coffeegameasset.h"
#include "general/data/numberbuffer.h"
class CoffeeMesh : public QObject, public CoffeeGameAsset{
    Q_OBJECT

    Q_PROPERTY(GLint vaoHandle READ vaoHandle WRITE setVaoHandle NOTIFY vaoHandleChanged)
    Q_PROPERTY(GLint vboHandle READ vboHandle WRITE setVboHandle NOTIFY vboHandleChanged)
    Q_PROPERTY(bool streamDraw READ streamDraw WRITE setStreamDraw NOTIFY streamDrawChanged)
    Q_PROPERTY(bool depthTest READ depthTest WRITE setDepthTest NOTIFY depthTestChanged)
    Q_PROPERTY(bool baked READ baked WRITE setBaked NOTIFY bakedChanged)
    Q_PROPERTY(bool drawn READ drawn WRITE setDrawn NOTIFY drawnChanged)

public:
    CoffeeMesh(QObject* parent);
    CoffeeMesh(QPointer<CoffeeMesh> mesh);
    QList<QPointer<CoffeeVertex> > copy();
    NumberBuffer<GLfloat>* getData();
    int getVerticesDataSize();
    int getVerticesSize();

    QList<glm::vec3> raw_vertices;
    QList<glm::vec2> raw_texcoords;

    QList<QPointer<CoffeeVertex> > getVertices();
    void addVertex(QPointer<CoffeeVertex> vert);

    void setVertices(QList<QPointer<CoffeeVertex> > vertices);

    void unloadMesh();

    QList<QPointer<CoffeeVertex> > vertices;

    GLint vboHandle() const;

    GLint vaoHandle() const;

    bool streamDraw() const;

    bool depthTest() const;

    bool baked() const;

    bool drawn() const;
public slots:
    void setVboHandle(GLint arg);

    void setVaoHandle(GLint arg);

    void setStreamDraw(bool arg);

    void setDepthTest(bool arg);

    void setBaked(bool arg);

    void setDrawn(bool arg);

signals:
    void vboHandleChanged(GLint arg);

    void vaoHandleChanged(GLint arg);

    void streamDrawChanged(bool arg);

    void depthTestChanged(bool arg);

    void bakedChanged(bool arg);

    void drawnChanged(bool arg);

private:
    GLint m_vboHandle = 0;
    GLint m_vaoHandle = 0;
    bool m_streamDraw = false;
    bool m_depthTest = true;
    bool m_baked = false;
    bool m_drawn = true;
};

#endif // COFFEEMESH

