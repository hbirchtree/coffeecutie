#include "coffeestandardobject.h"

CoffeeStandardObject::CoffeeStandardObject(QObject *parent) : CoffeeObject(parent)
{
}

void CoffeeStandardObject::render()
{

}

void CoffeeStandardObject::unload()
{

}

void CoffeeStandardObject::load()
{
    if(pmesh)
        pmesh->loadMesh();
}

CoffeeMesh *CoffeeStandardObject::mesh()
{
    return pmesh;
}

ShaderContainer *CoffeeStandardObject::shader()
{
    return pshader;
}

CoffeeMaterial *CoffeeStandardObject::material()
{
    return pmaterial;
}

