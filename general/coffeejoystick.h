#ifndef COFFEEJOYSTICK_H
#define COFFEEJOYSTICK_H

#include "general/common.h"

class CoffeeJoystick : public QObject
{
    Q_OBJECT
public:
    CoffeeJoystick(QObject *parent,int joystick);
    ~CoffeeJoystick();

    bool update();
    QString getJoystickName() const;

    QList<float> getAxes() const;

    QList<bool> getButtons() const;

signals:

private:
    int joystick;
    QString joystickName;
    QList<float> axes;
    QList<bool> buttons;
};

#endif // COFFEEJOYSTICK_H
