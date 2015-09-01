#ifndef COFFEERENDERCALL_H
#define COFFEERENDERCALL_H

#include <QObject>

class CoffeeRenderCall : public QObject
{
    Q_OBJECT
public:
    CoffeeRenderCall(QObject *parent,std::function<void()> *func);
    ~CoffeeRenderCall();

    std::function<void()> *m_funcVar = nullptr;
    bool m_executed = false;

public slots:
};

#endif // COFFEERENDERCALL_H
