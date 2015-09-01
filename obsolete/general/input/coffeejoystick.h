#ifndef COFFEEJOYSTICK_H
#define COFFEEJOYSTICK_H

#include "general/common.h"

class CoffeeHapticDevice : public QObject
{
};

class CoffeeJoystick : public QObject
{
    Q_PROPERTY(bool hasHapticDevice READ hasHapticDevice)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QString deviceName READ getJoystickName)
    Q_PROPERTY(int axes READ getAxes)
    Q_PROPERTY(int buttons READ getButtons)
    Q_PROPERTY(float deadzone READ getDeadzone)
    Q_PROPERTY(QVariantList axesValues READ axesValues)
    Q_PROPERTY(QVariantList btnValues READ btnValues)

    Q_OBJECT
public:
    virtual bool update() = 0;
    QString getJoystickName() const;

    float getDeadzone() const;
    void setDeadzone(float value);

    int getAxes() const;

    int getButtons() const;

    QVariantList axesValues() const;
    QVariantList btnValues() const;

    bool enabled() const;

    virtual bool hasHapticDevice() const = 0;

public slots:
    void setEnabled(bool enabled);

signals:
    void axisMoved(int axis, float value, float diff);
    void buttonPressed(int button);
    void buttonReleased(int button);

protected:
    CoffeeJoystick(QObject* parent);

    float j_deadzone = 0.22;
    int joystick;
    QString j_name;
    QVector<float> axes;
    QVector<unsigned char> buttons;
    bool m_enabled = true;
};

class CoffeeGLFWJoystick : public CoffeeJoystick
{
    Q_OBJECT
public:
    CoffeeGLFWJoystick(QObject *parent, int joystick);
    bool update();
    bool hasHapticDevice() const;
};

//SDL2 joystick implementation

class CoffeeSDL2Context; //The primary context for SDL events where they are polled and etc.

class CoffeeSDLJoystick : public CoffeeJoystick
{
    Q_OBJECT
public:
    CoffeeSDLJoystick(QObject* parent, CoffeeSDL2Context *ctxt, uint index);
    bool update();
    bool hasHapticDevice() const;
};

#endif // COFFEEJOYSTICK_H
