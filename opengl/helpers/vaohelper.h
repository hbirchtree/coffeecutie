#ifndef VAOHELPER_H
#define VAOHELPER_H

#include <QObject>

class VAOHelper : public QObject
{
    Q_OBJECT
public:
    explicit VAOHelper(QObject *parent = 0);
    ~VAOHelper();

signals:

public slots:
};

#endif // VAOHELPER_H
