#ifndef COFFEE_FUNCTIONAL_H
#define COFFEE_FUNCTIONAL_H

#include "coffee.h"

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

}
}


#endif // COFFEE_FUNCTIONAL_H
