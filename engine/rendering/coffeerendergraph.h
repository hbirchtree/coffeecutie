#ifndef COFFEERENDERGRAPH_H
#define COFFEERENDERGRAPH_H

#include <QObject>

#include "opengl/components/shadercontainer.h"
#include "opengl/components/framebuffers/coffeeframebufferbaseclass.h"

#include "engine/objects/coffeeoutputsurface.h"

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
    void setRenderTarget(CoffeeFramebufferBaseClass* fb);

    void setRenderer(CoffeeRenderer* renderer);
    void clearRenderer();

    void queueRender();
    void includeObject(CoffeeObject *obj);

    void setRenderSurface(CoffeeOutputSurface *renderSurface);

private:
    QAtomicInt m_taskCount;
    //We store a connection to avoid updating pointers
    QVector<QMetaObject::Connection> m_renderConnections;
    bool m_renderer_connected = false;

    QHash<CoffeeShader*,CoffeeRenderGroup*> m_renderGroups;

    CoffeeFramebufferBaseClass* m_renderTarget;
    CoffeeOutputSurface* m_renderSurface;
};

#endif // COFFEERENDERGRAPH_H
