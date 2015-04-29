#ifndef COFFEESDLJOYSTICK_H
#define COFFEESDLJOYSTICK_H

#include <QObject>

class CoffeeSDLJoystick : public QObject
{
    Q_OBJECT
public:
    explicit CoffeeSDLJoystick(QObject *parent = 0);

signals:

public slots:
};

#endif // COFFEESDLJOYSTICK_H
