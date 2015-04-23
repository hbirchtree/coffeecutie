#ifndef COFFEESCENE_H
#define COFFEESCENE_H

#include <QObject>
#include <QList>
#include "opengl/rendering/renderableobject.h"

class CoffeeScene : public QObject
{
    Q_OBJECT
public:
    explicit CoffeeScene(QObject *parent = 0);
    ~CoffeeScene();

    QList<QPointer<RenderableObject> > getRenderables();
    void renderAll();

signals:

public slots:

private:
    QList<QPointer<RenderableObject> > renderables;
};

#endif // COFFEESCENE_H
