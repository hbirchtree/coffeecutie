#ifndef COFFEEVERTEX
#define COFFEEVERTEX

#include "general/common.h"
class CoffeeVertex : public QObject {
public:
    static const int VERTEX_COUNT = 3+2+3+3;
    static const int VERTEX_STRIDE = sizeof(GLfloat)*VERTEX_COUNT;
    CoffeeVertex(QObject* parent) : QObject(parent){}
    CoffeeVertex(QPointer<CoffeeVertex> vertex){
        this->texCoord = vertex->texCoord;
        this->position = vertex->position;
        this->tangent = vertex->tangent;
        this->normal = vertex->normal;

        this->color = vertex->color;
    }
    CoffeeVertex(const CoffeeVertex &vert) : QObject(vert.parent()){
        this->position = vert.position;
        this->texCoord = vert.texCoord;
        this->normal = vert.normal;
        this->tangent = vert.tangent;
        this->bitangent = vert.bitangent;

        this->color = vert.color;
    }

    glm::vec2 texCoord;
    glm::vec3 position;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 normal;

    glm::vec4 color;

    int getSize(){
        return VERTEX_STRIDE;
    }
};

#endif // COFFEEVERTEX

