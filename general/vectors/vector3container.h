#ifndef VECTOR3CONTAINER_H
#define VECTOR3CONTAINER_H

#include <QObject>

class Vector3Container : public QObject
{
    Q_OBJECT
public:
    explicit Vector3Container(QObject *parent = 0);
    ~Vector3Container();

signals:

public slots:
};

#endif // VECTOR3CONTAINER_H
