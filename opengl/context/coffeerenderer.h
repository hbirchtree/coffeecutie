#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include "coffeegpumonitor.h"
#include "coffeeglfwcontextmanager.h"

class RenderLoop;

class CoffeeRenderer :
        public CoffeeGLFWContextManager,
        public CoffeeGPUMonitor
{
    Q_OBJECT
public:
    CoffeeRenderer(QObject *parent);
    CoffeeRenderer(QObject *parent, int w, int h);
    CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state);
    CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state, QString windowTitle);
    ~CoffeeRenderer();

    int loop();

    virtual void getVideoMemoryUsage(qint32 *current, qint32 *total) const;

public slots:
    void requestMemoryCheck();
    void flushPipeline();

    void setClearColor(float r, float g, float b, float a);
};

#endif // COFFEERENDERER_H
