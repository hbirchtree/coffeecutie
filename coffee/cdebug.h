#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include <QDebug>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
void cDebug(uint8_t severity, const char* str, Arg... args){
    switch(severity){
    case 0:
        qDebug(str, args...);
        break;
    case 1:
        qWarning(str, args...);
        break;
    default:
        qFatal(str, args...);
        break;
    }
}

}
}

#endif // COFFEE_DEBUG

