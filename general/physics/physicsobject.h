#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <QObject>

class PhysicsObject : public QObject
{
    Q_OBJECT
public:
    explicit PhysicsObject(QObject *parent = 0);
    ~PhysicsObject();

signals:

public slots:
};

#endif // PHYSICSOBJECT_H
