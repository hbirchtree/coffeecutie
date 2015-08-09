#ifndef COFFEEGLFWCONTEXTMANAGER_H
#define COFFEEGLFWCONTEXTMANAGER_H

#include <QObject>
#include "opengl/context/coffeerendererbase.h"
#include "general/eventloop/coffeefunctionqueuerunner.h"

#include "glfwnativefunctions.h"
#include "general/input/coffeejoystick.h"

class CoffeeGLFWContextManager : public CoffeeRendererBase,
        public CoffeeFunctionQueueRunner
{
    Q_PROPERTY(double frameTime READ getLatestFrameTime)
    Q_PROPERTY(double loopTime READ getLoopTime)

    Q_OBJECT
protected:
    CoffeeGLFWContextManager(QObject* parent);

    bool shouldWindowClose();
    void swapBuffers();
    void pollEvents();

    void updateJoysticks();

    void setLatestFrameTime(double time);

public:
    QWindow* getWindowHandle();

    virtual ~CoffeeGLFWContextManager();

    bool isMouseGrabbed() const;

    double getLoopTime() const;
    double getLatestFrameTime() const;

    int init();

    Q_INVOKABLE QObject* getJoystickDevice(uint index);

public slots:
    void queueFunction(std::function<void ()> func);

    void setWindowTitle(const QString &value);
    void setWindowDimensions(const QSize &windowSize);
    void setSwapInterval(uint interval);

    void setMouseGrabbing(bool state);
    void setMousePosition(QPointF pos);
    void setWindowState(Qt::WindowState state);

    void requestWindowClose();

private:
    QList<CoffeeJoystick*> m_joysticks;
    GLFWwindow *setWindowedFullscreen(int monitor);
    GLFWwindow *setFullscreen(int monitor);
    GLFWwindow *setWindowed();

    GLFWwindow* window = nullptr;
    double m_frameTime = 0.0;

    int m_multisamples = 0;
};

#endif // COFFEEGLFWCONTEXTMANAGER_H
