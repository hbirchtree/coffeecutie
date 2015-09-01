#ifndef RENDERLOOP
#define RENDERLOOP

#include <QObject>
class RenderLoop : public QObject{
    Q_OBJECT
public:
    RenderLoop(QObject* parent) : QObject(parent)
    {
    }
    ~RenderLoop(){}

    virtual std::function<void()> *init() = 0;
    virtual std::function<void()> *loop() = 0;
    virtual std::function<void()> *cleanup() = 0;
};

#endif // RENDERLOOP
