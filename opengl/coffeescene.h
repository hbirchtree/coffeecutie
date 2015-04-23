#ifndef COFFEESCENE_H
#define COFFEESCENE_H

#include <QObject>

class CoffeeScene : public QObject
{
    Q_OBJECT
public:
    explicit CoffeeScene(QObject *parent = 0);
    ~CoffeeScene();

signals:

public slots:
};

#endif // COFFEESCENE_H
