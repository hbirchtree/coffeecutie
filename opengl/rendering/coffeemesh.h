#ifndef COFFEEMESH
#define COFFEEMESH

#include "coffeevertex.h"
#include <QList>
class CoffeeMesh : public QObject{
    Q_PROPERTY(GLint vaoHandle READ vaoHandle WRITE setVaoHandle NOTIFY vaoHandleChanged)
    Q_PROPERTY(GLint vboHandle READ vboHandle WRITE setVboHandle NOTIFY vboHandleChanged)
    Q_PROPERTY(bool streamDraw READ streamDraw WRITE setStreamDraw NOTIFY streamDrawChanged)
    Q_PROPERTY(bool depthTest READ depthTest WRITE setDepthTest NOTIFY depthTestChanged)
    Q_PROPERTY(bool baked READ baked WRITE setBaked NOTIFY bakedChanged)
    Q_PROPERTY(bool drawn READ drawn WRITE setDrawn NOTIFY drawnChanged)

    CoffeeMesh(){}
    CoffeeMesh(QPointer<CoffeeMesh> mesh){
        this->vertices = mesh->vertices;
    }
    QList<QPointer<CoffeeVertex> > copy(){
        QList<QPointer<CoffeeVertex> > copy;
        for(QPointer<CoffeeVertex> vert : vertices){
            copy.append(new CoffeeVertex(vert));
        }
        return copy;
    }
    GLfloat* getData(){
        GLfloat data[vertices.size()*CoffeeVertex::VERTEX_STRIDE];
        int size = 0;
        for(QPointer<CoffeeVertex> vert : vertices){
            data[size+0] = vert->position.x;
            data[size+1] = vert->position.y;
            data[size+2] = vert->position.z;
            data[size+3] = vert->texCoord.x;
            data[size+4] = vert->texCoord.y;
            data[size+5] = vert->normal.x;
            data[size+6] = vert->normal.y;
            data[size+7] = vert->normal.z;
            data[size+8] = vert->tangent.x;
            data[size+9] = vert->tangent.y;
            data[size+10] = vert->tangent.z;
            size+=CoffeeVertex::VERTEX_STRIDE;
        }
        return data;
    }
    int getDataSize(){
        return getVerticesSize()*sizeof(GLfloat);
    }
    int getVerticesSize(){
        return vertices.size()*CoffeeVertex::VERTEX_STRIDE;
    }

    QPointer<QList<QPointer<CoffeeVertex> >> getVertices(){
        return &vertices;
    }

    QList<QPointer<CoffeeVertex> > vertices;

public:
    GLint vboHandle() const
    {
        return m_vboHandle;
    }

    GLint vaoHandle() const
    {
        return m_vaoHandle;
    }

    bool streamDraw() const
    {
        return m_streamDraw;
    }

    bool depthTest() const
    {
        return m_depthTest;
    }

    bool baked() const
    {
        return m_baked;
    }

    bool drawn() const
    {
        return m_drawn;
    }

public slots:
    void setVboHandle(GLint arg)
    {
        if (m_vboHandle == arg)
            return;

        m_vboHandle = arg;
        emit vboHandleChanged(arg);
    }

    void setVaoHandle(GLint arg)
    {
        if (m_vaoHandle == arg)
            return;

        m_vaoHandle = arg;
        emit vaoHandleChanged(arg);
    }

    void setStreamDraw(bool arg)
    {
        if (m_streamDraw == arg)
            return;

        m_streamDraw = arg;
        emit streamDrawChanged(arg);
    }

    void setDepthTest(bool arg)
    {
        if (m_depthTest == arg)
            return;

        m_depthTest = arg;
        emit depthTestChanged(arg);
    }

    void setBaked(bool arg)
    {
        if (m_baked == arg)
            return;

        m_baked = arg;
        emit bakedChanged(arg);
    }

    void setDrawn(bool arg)
    {
        if (m_drawn == arg)
            return;

        m_drawn = arg;
        emit drawnChanged(arg);
    }

signals:
    void vboHandleChanged(GLint arg);

    void vaoHandleChanged(GLint arg);

    void streamDrawChanged(bool arg);

    void depthTestChanged(bool arg);

    void bakedChanged(bool arg);

    void drawnChanged(bool arg);

private:
    GLint m_vboHandle;
    GLint m_vaoHandle;
    bool m_streamDraw;
    bool m_depthTest;
    bool m_baked;
    bool m_drawn;
};

#endif // COFFEEMESH

