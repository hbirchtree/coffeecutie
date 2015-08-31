#ifndef CGLFWNATIVEFUNCS_H
#define CGLFWNATIVEFUNCS_H

#include <stdint.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class CGLFWNativeFuncs
{
    CGLFWNativeFuncs();
public:
    static uintptr_t glfwGetNativeWindowHandle(GLFWwindow* window);
};

#endif // CGLFWNATIVEFUNCS_H
