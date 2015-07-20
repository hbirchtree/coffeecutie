#ifndef COFFEERENDERGRAPH_H
#define COFFEERENDERGRAPH_H

#include <QObject>

#include "opengl/components/shadercontainer.h"

class CoffeeRenderer;
class CoffeeUniformSetter;
class CoffeeObject;

class CoffeeRenderGroup
{
    //A grouping of objects with the same shader. Use multidraw.
public:
    QVector<CoffeeObject*> m_objects;
    CoffeeShader* m_shader;
};

class CoffeeRenderGraph : public QObject
{
    Q_OBJECT
public:
    CoffeeRenderGraph(QObject *parent);

signals:
    void submitRenderCall(std::function<void()> *func);

public slots:
    void setRenderer(CoffeeRenderer* renderer);
    void clearRenderer();

    void queueRender();
    void includeObject(CoffeeObject *obj);

private:
    //We store a connection to avoid updating pointers
    QMetaObject::Connection m_renderConnect;
    bool m_renderer_connected = false;

    QHash<CoffeeShader*,CoffeeRenderGroup*> m_renderGroups;
};

#endif // COFFEERENDERGRAPH_H
