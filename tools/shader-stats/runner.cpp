// Compile shaders on whatever Mesa driver EGL picks and print its debug output.
//   runner <item>...   item = shader file, or vert+frag to link a pair
// Each driver message is printed as "<item>\t<message>", and "### <item>" goes
// to stderr before each compile for drivers that write stats there. Stage is taken from
// the extension (.vert .frag .comp .geom .tesc .tese), API from #version.
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#ifndef EGL_PLATFORM_SURFACELESS_MESA
#define EGL_PLATFORM_SURFACELESS_MESA 0x31DD
#endif

#define GL_FN(name) static decltype(&::name) p_##name;
#define GL_FNS(X)                                                              \
    X(glGetString) X(glCreateShaderProgramv) X(glGetProgramiv)                 \
    X(glGetProgramInfoLog) X(glDeleteProgram) X(glCreateShader)                \
    X(glShaderSource) X(glCompileShader) X(glGetShaderiv)                      \
    X(glGetShaderInfoLog) X(glCreateProgram) X(glAttachShader)                 \
    X(glLinkProgram) X(glDeleteShader) X(glEnable) X(glDebugMessageCallback)   \
    X(glDebugMessageControl)
GL_FNS(GL_FN)

static std::string current;

static void GL_APIENTRY on_debug(
    GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* msg, const void*)
{
    std::string line(msg);
    while(!line.empty() && line.back() == '\n')
        line.pop_back();
    std::cout << current << '\t' << line << '\n';
}

static std::string slurp(std::string const& path)
{
    std::ifstream     f(path);
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}

static GLenum stage_of(std::string const& path)
{
    static const std::map<std::string, GLenum> stages = {
        {"vert", GL_VERTEX_SHADER},
        {"frag", GL_FRAGMENT_SHADER},
        {"comp", GL_COMPUTE_SHADER},
        {"geom", GL_GEOMETRY_SHADER},
        {"tesc", GL_TESS_CONTROL_SHADER},
        {"tese", GL_TESS_EVALUATION_SHADER},
    };
    auto ext = path.substr(path.rfind('.') + 1);
    auto it  = stages.find(ext);
    return it == stages.end() ? GL_FRAGMENT_SHADER : it->second;
}

static void report_failure(GLuint prog)
{
    GLint len = 0;
    p_glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    p_glGetProgramInfoLog(prog, len, nullptr, log.data());
    std::cout << current << "\tERROR\t" << log.substr(0, 400) << '\n';
}

static void run_single(std::string const& path)
{
    auto        src  = slurp(path);
    const char* ptr  = src.c_str();
    GLuint      prog = p_glCreateShaderProgramv(stage_of(path), 1, &ptr);
    GLint       ok   = 0;
    p_glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok)
        report_failure(prog);
    p_glDeleteProgram(prog);
}

static void run_pair(std::vector<std::string> const& paths)
{
    GLuint prog = p_glCreateProgram();
    for(auto const& path : paths)
    {
        auto        src = slurp(path);
        const char* ptr = src.c_str();
        GLuint      sh  = p_glCreateShader(stage_of(path));
        p_glShaderSource(sh, 1, &ptr, nullptr);
        p_glCompileShader(sh);
        p_glAttachShader(prog, sh);
        p_glDeleteShader(sh);
    }
    p_glLinkProgram(prog);
    GLint ok = 0;
    p_glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok)
        report_failure(prog);
    p_glDeleteProgram(prog);
}

static bool make_context(EGLDisplay dpy, bool es)
{
    eglBindAPI(es ? EGL_OPENGL_ES_API : EGL_OPENGL_API);
    const std::vector<std::pair<int, int>> versions =
        es ? std::vector<std::pair<int, int>>{{3, 2}, {3, 1}, {3, 0}, {2, 0}}
           : std::vector<std::pair<int, int>>{{4, 6}, {4, 3}, {4, 1}, {3, 3}};
    for(auto [major, minor] : versions)
    {
        std::vector<EGLint> attrs = {
            EGL_CONTEXT_MAJOR_VERSION, major,
            EGL_CONTEXT_MINOR_VERSION, minor,
            EGL_CONTEXT_OPENGL_DEBUG,  EGL_TRUE,
        };
        if(!es)
            attrs.insert(attrs.end(), {EGL_CONTEXT_OPENGL_PROFILE_MASK,
                                       EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT});
        attrs.push_back(EGL_NONE);
        auto ctx = eglCreateContext(dpy, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, attrs.data());
        if(ctx && eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx))
            return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "usage: runner <shader|vert+frag>...\n";
        return 1;
    }
    std::vector<std::string> items(argv + 1, argv + argc);
    auto first   = items.front().substr(0, items.front().find('+'));
    bool es      = slurp(first).find(" es") != std::string::npos;

    auto get_display = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
        eglGetProcAddress("eglGetPlatformDisplayEXT"));
    EGLDisplay dpy =
        get_display(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr);
    if(!eglInitialize(dpy, nullptr, nullptr) || !make_context(dpy, es))
    {
        std::cerr << "no EGL context (eglGetError 0x" << std::hex << eglGetError() << ")\n";
        return 2;
    }

#define GL_LOAD(name) p_##name = reinterpret_cast<decltype(&::name)>(eglGetProcAddress(#name));
    GL_FNS(GL_LOAD)
    std::cerr << "renderer: " << p_glGetString(GL_RENDERER) << " / "
              << p_glGetString(GL_VERSION) << '\n';

    p_glEnable(GL_DEBUG_OUTPUT);
    p_glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    p_glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    p_glDebugMessageCallback(on_debug, nullptr);

    for(auto const& item : items)
    {
        current = item.substr(item.rfind('/') + 1);
        // Drivers that print stats to stderr (panfrost, lima) need to know which shader
        std::cerr << "### " << current << std::endl;
        std::vector<std::string> parts;
        std::stringstream        ss(item);
        for(std::string part; std::getline(ss, part, '+');)
            parts.push_back(part);
        if(parts.size() == 1 && p_glCreateShaderProgramv)
            run_single(parts.front());
        else
            run_pair(parts);
        std::cout.flush();
    }
    return 0;
}
