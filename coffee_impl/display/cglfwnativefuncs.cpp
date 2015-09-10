#include "cglfwnativefuncs.h"

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "GLFW/glfw3native.h"
#include "plat/linux_wm.h"

#endif

//Something something compiler hints...
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3native.h"
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#include "GLFW/glfw3native.h"
#endif

#include "GLFW/glfw3native.h"

namespace Coffee{
namespace CDisplay{

CGLFWNativeFuncs::CGLFWNativeFuncs()
{

}

uintptr_t CGLFWNativeFuncs::glfwGetNativeWindowHandle(GLFWwindow* window)
{
#ifdef __linux__
    return glfwGetX11Window(window);
#endif
    return 0;
}

void CGLFWNativeFuncs::glfwSetDecorations(GLFWwindow *window, bool mode)
{
    uintptr_t ptr = glfwGetNativeWindowHandle(window);

#ifdef __linux__
    CoffeePlat::X11::set_decorations(glfwGetX11Display(),ptr,mode);
#endif
}

void CGLFWNativeFuncs::glfwSetFloating(GLFWwindow *window, bool mode)
{
    uintptr_t ptr = glfwGetNativeWindowHandle(window);

#ifdef __linux__
    CoffeePlat::X11::set_alwaysontop(glfwGetX11Display(),ptr,mode);
#endif
}

}
}
