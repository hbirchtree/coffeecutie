#ifndef COFFEESCENE_H
#define COFFEESCENE_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"

class CoffeeScene : public QObject
{
    Q_OBJECT
public:
    CoffeeScene(QObject *parent);
    ~CoffeeScene();

    QList<QPointer<RenderableObject> > getRenderables();
    void renderAll();

signals:

public slots:

private:
    QList<QPointer<RenderableObject> > renderables;
};

#endif // COFFEESCENE_H
