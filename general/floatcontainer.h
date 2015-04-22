#ifndef FLOATCONTAINER_H
#define FLOATCONTAINER_H

#include <QObject>

class FloatContainer : public QObject
{
    Q_OBJECT
public:
    explicit FloatContainer(QObject *parent = 0);
    ~FloatContainer();

signals:

public slots:
};

#endif // FLOATCONTAINER_H
