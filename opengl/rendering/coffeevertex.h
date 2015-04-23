#ifndef COFFEEVERTEX
#define COFFEEVERTEX

#include <QObject>
#include <QPointer>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <GL/glew.h>
class CoffeeVertex : public QObject {
    const int VERTEX_STRIDE = sizeof(float)*(3+2+3+3);
    CoffeeVertex(){

    }
    CoffeeVertex(CoffeeVertex* vertex){
        this->texCoord = vertex->texCoord;
        this->position = vertex->position;
        this->tangent = vertex->tangent;
        this->normal = vertex->normal;

        this->color = vertex->color;
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

