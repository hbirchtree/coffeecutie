#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <ctime>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
std::string cStringFormat(const char* fmt, Arg... args);

template<typename... Arg>
void cDebug(uint8_t severity, const char* str, Arg... args){
    time_t t;
    struct tm *tm;
    char clock[10];
    time(&t);
    tm = localtime(&t);
    strftime(clock,sizeof(clock),"%H:%M:%S:",tm);

    FILE* strm = stdout;
    bool fail = false;
    std::string s_(clock);

    switch(severity){
    case 0: //Info
        s_ += "INFO:";
        strm = stderr;
        break;
    case 1: //Debug
        s_ += "DEBG:";
        strm = stderr;
        break;
    case 2: //Warning
        s_ += "WARN:";
        strm = stderr;
        break;
    case 3:{ //Fatal
        s_ += "FTAL:";
        strm = stderr;
        fail = true;
        break;
    }
    }

    s_ += str;
    s_ += "\n";

    if(fail)
        throw std::runtime_error(cStringFormat(s_.c_str(),args...));
    else
        fprintf(strm,s_.c_str(),args...);
}

template<typename... Arg>
void cMsg(const char* src, const char* msg, Arg... args){
    //Message logger stuff, centralized for convenience
    std::string m = src;
    m += ":";
    m += msg;
    //I have a lot to learn about std::string
    cDebug(0,m.c_str(),args...);
}

template<typename... Arg>
std::string cStringFormat(const char* fmt, Arg... args){
    char* _s = new char[strlen(fmt)];
    sprintf(_s,fmt,args...);
    std::string _o(_s);
    delete[] _s;
    return _o;
}

}
}

#endif // COFFEE_DEBUG

