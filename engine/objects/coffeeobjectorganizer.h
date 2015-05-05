#ifndef COFFEEOBJECTORGANIZER_H
#define COFFEEOBJECTORGANIZER_H

#include "general/common.h"
#include "opengl/components/coffeeobject.h"

class CoffeeObjectOrganizer : public QObject
{
    Q_OBJECT
public:
    CoffeeObjectOrganizer(QObject *parent);

    bool createObject(QString name);
    CoffeeObject* getObject(QString name);
    int removeObject(QString name);

private:
    QHash<QString,QPointer<CoffeeObject>> objects;
};

#endif // COFFEEOBJECTORGANIZER_H
