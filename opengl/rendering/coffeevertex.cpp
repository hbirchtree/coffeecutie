#include "coffeevertex.h"


CoffeeVertex::CoffeeVertex(QPointer<CoffeeVertex> vertex){
    this->texCoord = vertex->texCoord;
    this->position = vertex->position;
    this->tangent = vertex->tangent;
    this->normal = vertex->normal;

    this->color = vertex->color;
}

CoffeeVertex::CoffeeVertex(const CoffeeVertex &vert) : QObject(vert.parent()){
    this->position = vert.position;
    this->texCoord = vert.texCoord;
    this->normal = vert.normal;
    this->tangent = vert.tangent;
    this->bitangent = vert.bitangent;

    this->color = vert.color;
}

int CoffeeVertex::getSize(){
    return VERTEX_STRIDE;
}

QString CoffeeVertex::toString()
{
    return toString(this);
}

QString CoffeeVertex::toString(const CoffeeVertex &v)
{
    return QString("CoffeeVertex(pt=%1,tx=%2,nl=%3,tn=%4)")
            .arg(QStringFunctions::toString(v.position))
            .arg(QStringFunctions::toString(v.texCoord))
            .arg(QStringFunctions::toString(v.normal))
            .arg(QStringFunctions::toString(v.tangent));
}
