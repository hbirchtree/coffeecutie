#ifndef QSTRINGFUNCTIONS_H
#define QSTRINGFUNCTIONS_H

#include "general/common.h"
class QStringFunctions
{
private:
    QStringFunctions();

public:

    static QString toString(const glm::vec2 &v);
    static QString toString(const glm::vec3 &v);
    static QString toString(const glm::vec4 &v);

    static QString toString(const glm::quat &q);

    static QString toString(const glm::mat3 &m);
    static QString toString(const glm::mat4 &m);
};

#endif // QSTRINGFUNCTIONS_H
