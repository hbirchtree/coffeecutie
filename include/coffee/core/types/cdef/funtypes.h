#pragma once

#include "../../coffee_mem_macros.h"

#include "../tdef/stltypes.h"
#include <functional>

namespace Coffee{

template<class ClassName,typename FReturnType,typename... FArgumentTypes>
struct CFunctionSlot
{
    using ClassType = ClassName;
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(ClassType::*)(FArgumentTypes...);
    using FunctionConst = FunctionReturn(ClassType::*)(FArgumentTypes...) const;

    FORCEDINLINE CFunctionSlot(ClassType* c, Function f):
        instance(c),n(f)
    {
    }
    FORCEDINLINE CFunctionSlot(ClassType* c, FunctionConst f):
        instance(c),c(f)
    {
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg)
    {
        return (instance->*n)(arg...);
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg) const
    {
        return (instance->*c)(arg...);
    }

    ClassName* instance;
    union{
        Function n;
        FunctionConst c;
    };
};

template<class ClassName, typename FReturnType, typename... FArgumentTypes>
/*!
 * \brief Can be extended for indirect calls to functions, either depending on thread or whatever.
 */
struct CFunctionSignal
{
    using ClassType = ClassName;
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(ClassType::*)(FArgumentTypes...);

    virtual FunctionReturn call(
            CFunctionSlot<ClassType,FReturnType,FArgumentTypes...> f,
            FArgumentTypes... args)
    {
        return f.call(args...);
    }
};

template<typename FReturnType,typename... FArgumentTypes>
struct CStaticFunctionBinding
{
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(*)(FArgumentTypes...);

    FORCEDINLINE CStaticFunctionBinding(Function f):
        function(f)
    {
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg)
    {
        return function(arg...);
    }

    Function function;
};

/* Job system-related */

class _cbasic_threadrunner_command
{
public:
    STATICINLINE void perform(_cbasic_threadrunner_command*){}
    STATICINLINE void await(_cbasic_threadrunner_command*,uint64){}
};

class _cbasic_threadrunner_queue
{
public:
    virtual void processEvents() = 0;
    void insertCmd(_cbasic_threadrunner_command*){}
};

class CThreadCommand : public _cbasic_threadrunner_command
{
public:
    CThreadCommand(std::function<void()> f):m_cmd(f){}
    STATICINLINE void perform(CThreadCommand* c)
    {
        c->m_cmd();
    }
    virtual bool await(uint64 = 0)
    {
        return m_stat.load();
    }
protected:
    std::function<void()> m_cmd;
    std::atomic_bool m_stat;
};

/*!
 * \brief Event loop for inserting commands from multiple threads, reimplement for
 */
class CEventLoop
{
public:
    virtual void processEvents()
    {
        m_cmdlistaccess.lock();
        while(!m_cmds.empty())
        {
            CThreadCommand::perform(m_cmds.front());
            m_cmds.pop();
        }
        m_cmdlistaccess.unlock();
    }
    FORCEDINLINE void insertCmd(CThreadCommand* c)
    {
        m_cmdlistaccess.lock();
        m_cmds.push(c);
        m_cmdlistaccess.unlock();
    }
protected:
    Mutex m_cmdlistaccess;
    Queue<CThreadCommand*> m_cmds;
};

}