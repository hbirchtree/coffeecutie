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

template<typename... Arg>
void cMsg(const char* src, const char* msg, Arg... args){
    //Message logger stuff, centralized for convenience
    qDebug(msg,args...);
}

template<typename... Arg>
std::string cStringFormat(const char* fmt, Arg... args){
    char* _s = new char[strlen(fmt)];
    sprintf(_s,fmt,args...);
    std::string _o = _s;
    delete[] _s;
    return _o;
}

}
}

#endif // COFFEE_DEBUG

