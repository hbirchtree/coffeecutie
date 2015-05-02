#ifndef QCOFFEERENDERER_H
#define QCOFFEERENDERER_H

#include "opengl/coffeerenderer.h"
#include "opengl/qcoffeewindow.h"

class QCoffeeRenderer : public CoffeeRenderer
{
public:
    QCoffeeRenderer(QObject *parent);
    QCoffeeRenderer(QObject *parent,int w, int h);
    QCoffeeRenderer(QObject *parent,int w, int h, Qt::WindowState state);
    QCoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state,QString windowTitle);

    int init();
    int loop();
private:
    QPointer<QCoffeeWindow> window;

};

#endif // QCOFFEERENDERER_H
