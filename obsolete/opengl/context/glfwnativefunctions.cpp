#include "glfwnativefunctions.h"

#include <QDebug>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#if defined(Q_OS_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "X11/X.h"

#elif defined(Q_OS_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#elif defined(Q_OS_MAC)
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL

#endif

#include "GLFW/glfw3native.h"


QWindow *GLFWNativeFunctions::getQWindow(GLFWwindow* window)
{
    QWindow* win = nullptr;

#if defined(Q_OS_LINUX)

    Window xwin = glfwGetX11Window(window);
    win = QWindow::fromWinId(xwin);

#endif

    return win;
}
