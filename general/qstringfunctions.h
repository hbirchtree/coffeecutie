#ifndef QSTRINGFUNCTIONS_H
#define QSTRINGFUNCTIONS_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/rendering/coffeevertex.h"
#include "general/data/numberbuffer.h"
class QStringFunctions
{
private:
    QStringFunctions();

public:
//    static QString toString(NumberBuffer<float> &b);

    static QString toString(CoffeeVertex &v);
//    static QString toString(RenderableObject &o);

    static QString toString(glm::vec2 &v);
    static QString toString(glm::vec3 &v);
    static QString toString(glm::vec4 &v);

    static QString toString(glm::quat &q);

    static QString toString(glm::mat3 &m);
    static QString toString(glm::mat4 &m);
};

#endif // QSTRINGFUNCTIONS_H
