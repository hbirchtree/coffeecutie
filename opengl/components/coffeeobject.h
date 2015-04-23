#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include <QObject>

class CoffeeObject : public QObject
{
    Q_OBJECT
public:
    explicit CoffeeObject(QObject *parent = 0);
    ~CoffeeObject();

signals:

public slots:
};

#endif // COFFEEOBJECT_H
