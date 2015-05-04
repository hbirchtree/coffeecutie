#ifndef COFFEEJOYSTICK_H
#define COFFEEJOYSTICK_H

#include "general/common.h"

class CoffeeJoystick : public QObject
{
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

signals:
    void axisMoved(int axis, float value);
    void buttonPressed(int button);
    void buttonReleased(int button);

private:
    float j_axisfactor = 32768;
    float j_sensitivity = 0.01;
    float j_deadzone = 0.15;
    int joystick;
    QString j_name;
    QList<float> axes;
    QList<unsigned char> buttons;
};

#endif // COFFEEJOYSTICK_H
