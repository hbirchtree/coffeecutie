#include "coffeerendercall.h"

CoffeeRenderCall::CoffeeRenderCall(QObject *parent, std::function<void()> *func) :
    QObject(parent)
{
    m_funcVar = func;
}

CoffeeRenderCall::~CoffeeRenderCall()
{
    delete m_funcVar;
}
