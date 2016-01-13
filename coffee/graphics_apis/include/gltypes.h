#ifndef COFFEE_GRAPICS_APIS_GLCORE_GLTYPES_H
#define COFFEE_GRAPICS_APIS_GLCORE_GLTYPES_H

#include <base/cthreading.h>

namespace Coffee{
namespace CGL{

/*!
 * \brief An objectified OpenGL context
 */
struct CGL_Context
{
    virtual ~CGL_Context(){}
    virtual bool acquireContext() = 0;
    virtual bool releaseContext() = 0;
    virtual const CFunctional::CThreading::ThreadId_t &currentThread() = 0;
};
/*!
 * \brief A static object
 */
struct CGL_Layer
{
    using InterceptFunction = void(*)(cstring);

    static bool Initialize(){return false;}
    static bool Terminate(){return false;}

    static void InsertCallInterceptPre(InterceptFunction){}
    static void InsertCallInterceptPost(InterceptFunction){}

    static CGL_Context* CreateContext(){return nullptr;}
    static bool DeleteContext(CGL_Context*){return false;}
};
/*!
 * \brief A thread-local scoped OpenGL context, acquired on creation and released on deletion
 */
struct CGL_ScopedContext
{
    CGL_ScopedContext(CGL_Context* ctxt):
        m_ctxt(ctxt)
    {
        m_ctxt->acquireContext();
    }
    virtual ~CGL_ScopedContext()
    {
        m_ctxt->releaseContext();
    }
protected:
    CGL_Context* m_ctxt;
};

}
}

#endif
