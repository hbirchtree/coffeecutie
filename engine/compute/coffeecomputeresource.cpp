#include "coffeecomputeresource.h"

CoffeeComputeResource::CoffeeComputeResource(QObject *parent) : QObject(parent)
{

}

CoffeeComputeResource::ComputeType CoffeeComputeResource::type() const
{
    return m_type;
}

