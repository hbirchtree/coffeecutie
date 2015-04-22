#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include <QObject>

class CoffeeRenderer : public QObject
{
    Q_OBJECT
public:
    explicit CoffeeRenderer(QObject *parent = 0);
    ~CoffeeRenderer();

signals:

public slots:
};

#endif // COFFEERENDERER_H
