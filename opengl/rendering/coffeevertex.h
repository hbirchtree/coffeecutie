#ifndef COFFEEVERTEX
#define COFFEEVERTEX

#include "general/common.h"
class CoffeeVertex : public QObject {
public:
    static const int VERTEX_COUNT = 3+2+3+3;
    static const int VERTEX_STRIDE = sizeof(GLfloat)*VERTEX_COUNT;
    CoffeeVertex(QObject* parent) : QObject(parent){}
    CoffeeVertex(QPointer<CoffeeVertex> vertex);
    CoffeeVertex(const CoffeeVertex &vert);

    glm::vec2 texCoord;
    glm::vec3 position;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 normal;

    glm::vec4 color;

    int getSize();

    QString toString();
    static QString floatBufferToString(GLfloat* buf, int from, int to);
};

#endif // COFFEEVERTEX

