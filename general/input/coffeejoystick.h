#ifndef COFFEEJOYSTICK_H
#define COFFEEJOYSTICK_H

#include "general/common.h"

class CoffeeJoystick : public QObject
{
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QString deviceName READ getJoystickName)
    Q_PROPERTY(int axes READ getAxes)
    Q_PROPERTY(int buttons READ getButtons)
    Q_PROPERTY(float sensitivity READ getSensitivity)
    Q_PROPERTY(float deadzone READ getDeadzone)
    Q_PROPERTY(QVariantList axesValues READ axesValues)
    Q_PROPERTY(QVariantList btnValues READ btnValues)

    Q_OBJECT
public:
    CoffeeJoystick(QObject *parent, int joystick);
    ~CoffeeJoystick();

    bool update();
    QString getJoystickName() const;

    float getSensitivity() const;
    void setSensitivity(float value);

    float getDeadzone() const;
    void setDeadzone(float value);

    float getAxisfactor() const;
    void setAxisfactor(float value);

    int getAxes() const;

    int getButtons() const;

    QVariantList axesValues() const;
    QVariantList btnValues() const;

    bool enabled() const;

public slots:
    void setEnabled(bool enabled);

signals:
    void axisMoved(int axis, float value, float diff);
    void buttonPressed(int button);
    void buttonReleased(int button);

private:
    float j_axisfactor = 32768;
    float j_sensitivity = 0.01;
    float j_deadzone = 0.22;
    int joystick;
    QString j_name;
    QVector<float> axes;
    QVector<unsigned char> buttons;
    bool m_enabled;
};

#endif // COFFEEJOYSTICK_H
