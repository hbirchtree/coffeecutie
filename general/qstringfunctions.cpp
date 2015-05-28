#include "qstringfunctions.h"

QStringFunctions::QStringFunctions()
{

}

QString QStringFunctions::toString(const glm::vec2 &v)
{
    return QString("glm::vec2[%1,%2]")
            .arg(v.x)
            .arg(v.y);
}

QString QStringFunctions::toString(const glm::vec3 &v)
{
    return QString("glm::vec3[%1,%2,%3]")
            .arg(v.x)
            .arg(v.y)
            .arg(v.z);
}

QString QStringFunctions::toString(const glm::vec4 &v)
{
    return QString("glm::vec4[%1,%2,%3,%4]")
            .arg(v.x)
            .arg(v.y)
            .arg(v.z)
            .arg(v.w);
}

QString QStringFunctions::toString(const glm::quat &q)
{
    return QString("glm::quat[%1,%2,%3,%4]")
            .arg(q.w)
            .arg(q.x)
            .arg(q.y)
            .arg(q.z);
}

QString QStringFunctions::toString(const glm::mat3 &m)
{
    return QString("glm::mat3\n"
                   "[%1,%2,%3]\n"
                   "[%4,%5,%6]\n"
                   "[%7,%8,%9]")
            .arg(m[0][0])
            .arg(m[0][1])
            .arg(m[0][2])
            .arg(m[1][0])
            .arg(m[1][1])
            .arg(m[1][2])
            .arg(m[2][0])
            .arg(m[2][1])
            .arg(m[2][2]);
}

QString QStringFunctions::toString(const glm::mat4 &m)
{
    return QString("glm::mat4\n"
                   "[%1,%2,%3,%4]\n"
                   "[%5,%6,%7,%8]\n"
                   "[%9,%10,%11,%12]\n"
                   "[%13,%14,%15,%16]")
            .arg(m[0][0])
            .arg(m[0][1])
            .arg(m[0][2])
            .arg(m[0][3])
            .arg(m[1][0])
            .arg(m[1][1])
            .arg(m[1][2])
            .arg(m[1][3])
            .arg(m[2][0])
            .arg(m[2][1])
            .arg(m[2][2])
            .arg(m[2][3])
            .arg(m[3][0])
            .arg(m[3][1])
            .arg(m[3][2])
            .arg(m[3][3]);
}
