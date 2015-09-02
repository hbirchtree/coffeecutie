#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>

#include <vector>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <ctime>
#include <iostream>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
std::string cStringFormat(const char* fmt, Arg... args);

static char* coffee_demangle_symbol(char* sym,bool* success){
    int stat;
    char* demangled = abi::__cxa_demangle(sym,nullptr,nullptr,&stat);
    if(stat == 0){
        *success = true;
        return demangled;
    }else{
        *success = false;
        std::free(demangled);
        return sym;
    }
}

static std::vector<std::string> coffee_callstack(){

    //TODO: Write this in pure C, much lighter than std::vector

    std::vector<std::string> res;

    unw_cursor_t cur;
    unw_context_t ctx;
    unw_getcontext(&ctx);
    unw_init_local(&cur,&ctx);

    unw_step(&cur);unw_step(&cur); //Remove the observer

    size_t step = unw_step(&cur);
    while(step>0){
        unw_word_t offset,pc;
        unw_get_reg(&cur,UNW_REG_IP,&pc);
        if(pc == 0)
            break;

        char sym[256];
        bool success;
        if(unw_get_proc_name(&cur,sym,sizeof(sym),&offset)==0){
            char* fname = coffee_demangle_symbol(sym,&success);
            res.push_back(std::string(fname));
            if(success)
                std::free(fname);
        }
        step = unw_step(&cur);
    }
    return res;
}

template<typename... Arg>
static void cDebug(uint8_t severity, const char* str, Arg... args){
    //Get time as string
    time_t t;
    struct tm *tm;
    char clock[10];
    time(&t);
    tm = localtime(&t);
    strftime(clock,sizeof(clock),"%H:%M:%S:",tm);

    //Some settings for output
    FILE* strm = stdout;
    bool fail = false;
    std::string s_(clock);

    //Get call stack
    std::vector<std::string> cs = coffee_callstack();
    std::string first = (cs.size()>0) ? cs[0] : "[callstack unavailable]";
    //
    s_ += first;
    s_ += ":";

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
static void cMsg(const char* src, const char* msg, Arg... args){
    //Message logger stuff, centralized for convenience
    std::string m = src;
    m += ":";
    m += msg;
    //I have a lot to learn about std::string
    cDebug(0,m.c_str(),args...);
}

template<typename... Arg>
static std::string cStringFormat(const char* fmt, Arg... args){
    char* _s = new char[strlen(fmt)];
    sprintf(_s,fmt,args...);
    std::string _o(_s);
    delete[] _s;
    return _o;
}

}
}

#endif // COFFEE_DEBUG

