#ifndef COFFEEGLFWCONTEXTMANAGER_H
#define COFFEEGLFWCONTEXTMANAGER_H

#include "opengl/context/coffeerendererbase.h"
#include "general/eventloop/coffeefunctionqueuerunner.h"

#include "glfwnativefunctions.h"
#include "general/input/coffeejoystick.h"

class CoffeeGLFWContextManager : public CoffeeRendererBase,
        public CoffeeFunctionQueueRunner
{
    Q_PROPERTY(double frameTime READ getLatestFrameTime)
    Q_PROPERTY(double loopTime READ getLoopTime)

    Q_PROPERTY(QString renderer READ renderer)
    Q_PROPERTY(QString vendor READ vendor)
    Q_PROPERTY(QString version READ version)

    Q_PROPERTY(QString contextManager READ contextManager)

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

    QWindow *windowHandle();

    int init();

    Q_INVOKABLE QObject* getJoystickDevice(uint index);

    QString renderer() const;
    QString vendor() const;
    QString version() const;
    QString contextManager() const;

public slots:
    void queueFunction(std::function<void ()> func);

    void setWindowTitle(const QString &value);
    void setWindowDimensions(const QSize &windowSize);
    void setSwapInterval(uint interval);

    void setMouseGrabbing(bool state);
    void setMousePosition(QPointF pos);
    void setWindowState(Qt::WindowState state);

    void requestWindowClose();

    void showWindow();
    void hideWindow();

private:
    QList<CoffeeJoystick*> m_joysticks;
    GLFWwindow *setWindowedFullscreen(int monitor,const char *title);
    GLFWwindow *setFullscreen(int monitor,const char *title);
    GLFWwindow *setWindowed(const char *title);

    GLFWwindow* window = nullptr;
    double m_frameTime = 0.0;

    int m_multisamples = 0;

    QString m_renderer;
    QString m_vendor;
    QString m_version;
    QString m_contextManager;
};

#endif // COFFEEGLFWCONTEXTMANAGER_H
