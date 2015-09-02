#ifndef COFFEE_FUNCTIONAL_H
#define COFFEE_FUNCTIONAL_H

#include "coffee.h"
#include <regex>

namespace Coffee{
namespace CFunctional{

template<typename RType, typename... AType>
class CRFunction {
public:
    virtual RType exec(AType... args) = 0;
};

template<class TClass, class FClass, typename RType, typename... AType>
class CRMemberFunction : public CRFunction<RType,AType...>{
    typedef RType (FClass::*FuncPtr)(AType...);

    TClass* m_instance;
    FuncPtr m_func;
public:
    CRMemberFunction(TClass* obj, RType (FClass::*fun)(AType...)){
        m_instance = obj;
        m_func = fun;
    }
    RType exec(AType... args){
        return (((FClass*)m_instance)->*m_func)(args...);
    }
};

template<typename RType,typename... AType>
class CRStaticFunction : public CRFunction<RType,AType...>{
    typedef RType (*FuncPtr)(AType...);

    FuncPtr m_func;
public:
    CRStaticFunction(FuncPtr fun){
        m_func = fun;
    }
    RType exec(AType... args){
        return (*m_func)(args...);
    }
};

struct CRegexMatch
{
    std::vector<std::string> s_match;
    bool b_match;
};

static std::vector<CRegexMatch> coffee_regex_match(const std::string& pattern,
                   const std::vector<std::string>& data,
                   bool capture = false)
{
    std::regex rgx(pattern);
    std::smatch mch;

    std::vector<CRegexMatch> matches;

    for(const std::string& string : data){
        CRegexMatch m;
        m.b_match = false;
        if(std::regex_match(string,mch,rgx)){
            if(capture){
                for(size_t i=0;i<mch.size();i++){
                    std::ssub_match smch = mch[i];
                    m.b_match = true;
                    m.s_match.push_back(smch.str());
                }
            }else{
                if(mch.size()==2){
                    std::ssub_match smch = mch[1];
                    m.b_match = true;
                    m.s_match.push_back(smch.str());
                }
            }
        }
        matches.push_back(m);
    }
    return matches;
}

}
}


#endif // COFFEE_FUNCTIONAL_H
