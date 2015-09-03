#include "cglfwnativefuncs.h"

#include <QObject>

#ifdef Q_OS_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "X11/X.h"
#endif

#ifdef Q_OS_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#ifdef Q_OS_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif

#include "GLFW/glfw3native.h"

namespace Coffee{
namespace CDisplay{

CGLFWNativeFuncs::CGLFWNativeFuncs()
{

}

uintptr_t CGLFWNativeFuncs::glfwGetNativeWindowHandle(GLFWwindow* window)
{
#ifdef Q_OS_LINUX
    return glfwGetX11Window(window);
#endif
    return 0;
}

}
}
