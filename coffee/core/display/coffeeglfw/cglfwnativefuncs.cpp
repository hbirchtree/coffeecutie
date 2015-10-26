#include "cglfwnativefuncs.h"

#include "plat/platform_detect.h"

#ifdef COFFEE_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "GLFW/glfw3native.h"
#include "plat/coffee_linux/linux_wm.h"

#endif

//Something something compiler hints...
#ifdef COFFEE_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3native.h"
#endif
#ifdef COFFEE_APPLE
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
#ifdef COFFEE_LINUX
    return glfwGetX11Window(window);
#endif
    return 0;
}

void CGLFWNativeFuncs::glfwSetDecorations(GLFWwindow *window, bool mode)
{
    uintptr_t ptr = glfwGetNativeWindowHandle(window);

#ifdef COFFEE_LINUX
    CoffeePlat::X11::set_decorations(glfwGetX11Display(),ptr,mode);
#endif
}

void CGLFWNativeFuncs::glfwSetFloating(GLFWwindow *window, bool mode)
{
    uintptr_t ptr = glfwGetNativeWindowHandle(window);

#ifdef COFFEE_LINUX
    CoffeePlat::X11::set_alwaysontop(glfwGetX11Display(),ptr,mode);
#endif
}

}
}
