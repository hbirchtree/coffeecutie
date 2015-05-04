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
    return QString("pt:%1,%2,%3 tx:%4,%5 nl:%6,%7,%8 tn:%9,%10,%11")
            .arg(position.x)
            .arg(position.y)
            .arg(position.z)
            .arg(texCoord.x)
            .arg(texCoord.y)
            .arg(normal.x)
            .arg(normal.y)
            .arg(normal.z)
            .arg(tangent.x)
            .arg(tangent.y)
            .arg(tangent.z);
}
