#ifndef QCOFFEEWINDOW_H
#define QCOFFEEWINDOW_H

#include <QWindow>
#include <QPointer>
#include <QExposeEvent>
#include <QPainter>
#include "opengl/rendering/renderloop.h"

class QOpenGLContext;
class QOpenGLPaintDevice;

class QCoffeeWindow : public QWindow
{
public:
    QCoffeeWindow(QWindow *parent, QSurfaceFormat format);
    ~QCoffeeWindow();

    void initialize();
    void render();
    void cleanup();

    QPointer<RenderLoop> getProcedure() const;
    void setProcedure(const QPointer<RenderLoop> &value);
private:
    bool m_allow_render = false;
    QSurfaceFormat format;
    QPointer<RenderLoop> procedure;
    QPointer<QOpenGLContext> context;
    QOpenGLPaintDevice* device;
};

#endif // QCOFFEEWINDOW_H
