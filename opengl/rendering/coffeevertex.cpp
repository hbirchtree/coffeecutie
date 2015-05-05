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
