#ifndef QCOFFEERENDERER_H
#define QCOFFEERENDERER_H

#include "opengl/context/coffeerenderer.h"
#include <QWindow>
#include <QPointer>
#include <QExposeEvent>
#include <QPainter>
#include "opengl/rendering/renderloop.h"

#ifdef QOPENGL_CONTEXT_MANAGER

class QCoffeeWindow : public QOpenGLWindow
{
public:
    QCoffeeWindow();
    ~QCoffeeWindow();

    void initialize();
    void render();
    void cleanup();

    QPointer<RenderLoop> getProcedure() const;
    void setProcedure(const QPointer<RenderLoop> &value);
private:
    QPointer<RenderLoop> procedure;

    // QWindow interface
protected:
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void touchEvent(QTouchEvent *);
    void tabletEvent(QTabletEvent *);

    // QOpenGLWindow interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void paintUnderGL();
    void paintOverGL();
};

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


#endif

#endif // QCOFFEERENDERER_H
