#include <glbinding/wrappers/cgraphicswrappers.h>

#include <glfunctions.h>

namespace Coffee{
namespace CGraphicsWrappers{

CGLReport::CGLReport():
    source(GL_NONE),
    type(GL_NONE),
    id(0),
    severity(GL_NONE),
    message(nullptr)
{
}

CGLDrawCall::CGLDrawCall():
    count(0),
    instanceCount(0),
    firstIndex(0),
    baseVertex(0),
    baseInstance(0)
{
}

void coffee_graphics_clear(CClearFlag flg)
{
    auto clearflags = GL_COLOR_BUFFER_BIT;

    if(feval(flg&CClearFlag::Depth))
        clearflags = clearflags|GL_DEPTH_BUFFER_BIT;
    if(feval(flg&CClearFlag::Color))
        clearflags = clearflags|GL_COLOR_BUFFER_BIT;
    if(feval(flg&CClearFlag::Stencil))
        clearflags = clearflags|GL_STENCIL_BUFFER_BIT;
    if(feval(flg&CClearFlag::Accum))
        clearflags = clearflags|GL_ACCUM_BUFFER_BIT;

    glClear(clearflags);
}

void coffee_graphics_draw_indexed(const CPrimitiveMode &mode, const CGLDrawCall &drawcall)
{
    glDrawElementsInstancedBaseVertexBaseInstance(
                gl_get(mode),
                drawcall.count,
                gl_get(CDataType::UInt),
                (void*)(sizeof(CGint)*drawcall.firstIndex),
                drawcall.instanceCount,
                drawcall.baseVertex,
                drawcall.baseInstance);
}

void coffee_graphics_multidraw(const CPrimitiveMode &mode, const CBuffer &buf)
{
    glMultiDrawElementsIndirect(
                gl_get(mode),
                gl_get(CDataType::UInt),
                0, buf.size/sizeof(CGLDrawCall),
                sizeof(CGLDrawCall));
}

void coffee_graphics_draw_arrays(const CPrimitiveMode &mode,
                                 const CGint& first, const CGint& count)
{
    glDrawArrays(gl_get(mode),first,count);
}

void coffee_graphics_depth(bool enable)
{
    if(enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void coffee_graphics_blend(bool enable,bool additive)
{
    if(enable)
    {
        glEnable(GL_BLEND);
        if(additive)
            glBlendFunc(GL_ONE,GL_ONE);
        else
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }else
        glDisable(GL_BLEND);
}

void coffee_graphics_culling(bool enable, CCullMode mode)
{
    if(enable)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(gl_get(mode));
    }
    else
        glDisable(GL_CULL_FACE);
}

void coffee_graphics_set_clearcolor(const CVec4 &color)
{
    glClearColor(color.r(),color.g(),color.b(),color.a());
}

void coffee_graphics_set_viewport(const CRectF &view)
{
    coffee_graphics_viewport_set(&view,0,1);
}

bool coffee_graphics_debug_filter_ignore_notifications(CGLDebugMessage const& rep)
{
    if(rep.severity == CDebugSeverity::Notification)
        return false;
    return true;
}

bool coffee_graphics_debug_filter_all(CGLDebugMessage const&)
{
    return true;
}

struct cg_debug_data
{
    CDebugCallback function;
    void* userPtr;
};

CGLDebugMessage coffee_graphics_debug_report(CGLReport const* report)
{
    CGLDebugMessage o;

    o.severity = gl_get_dbg_severity(report->severity);
    o.type = gl_get_dbg_type(report->type);
    o.source = gl_get_dbg_src(report->source);
    o.message = report->message;
    o.id = report->id;

    return o;
}

void coffee_graphics_debug_director(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei,
        const GLchar* msg,
        void* userPtr)
{
    if(!userPtr)
        return;

    CGLReport report;
    report.source = source;
    report.type = type;
    report.id = id;
    report.message = msg;
    report.severity = severity;

    CGLDebugMessage cmessage = coffee_graphics_debug_report(&report);

    cg_debug_data* data = (cg_debug_data*)userPtr;

    if(data->function)
        data->function(cmessage,data->userPtr);
}

static cg_debug_data coffee_dbg_data;

bool coffee_graphics_debug_context(bool enable, CDebugCallback userFunction, void* userPtr)
{
    if(enable)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    else{
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        return true;
    }

    coffee_dbg_data.function = userFunction;
    coffee_dbg_data.userPtr = userPtr;

    glDebugMessageCallback((GLDEBUGPROC)coffee_graphics_debug_director,&coffee_dbg_data);

    return true;
}

CString coffee_graphics_get_extensions(int32 *numExtensions_p)
{
    GLint numExtensions;

    numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS,&numExtensions);

    CString res;

    for(GLint i=0;i<numExtensions;i++)
    {
        cstring str = (cstring)glGetStringi(GL_EXTENSIONS,i);
        if(!str)
            continue;
        res.append(str);
        if(i<numExtensions-1)
            res.push_back('\n');
    }

    if(numExtensions_p)
        *numExtensions_p = numExtensions;

    return res;
}

bool coffee_graphics_check_extension(cstring ext, cstring source)
{
    return strstr(source,ext);
}

CGLVersion coffee_graphics_context_get_version()
{
    CGLVersion ver;
    glGetIntegerv(GL_MAJOR_VERSION,&ver.major);
    glGetIntegerv(GL_MINOR_VERSION,&ver.minor);
    ver.revision = 0;
    return ver;
}

cstring coffee_graphics_context_get_vendor()
{
    return (cstring)glGetString(GL_VENDOR);
}

cstring coffee_graphics_context_get_renderer()
{
    return (cstring)glGetString(GL_RENDERER);
}

cstring coffee_graphics_context_get_version_string()
{
    return (cstring)glGetString(GL_VERSION);
}

}
}
