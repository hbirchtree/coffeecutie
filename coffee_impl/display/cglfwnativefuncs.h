#ifndef CGLFWNATIVEFUNCS_H
#define CGLFWNATIVEFUNCS_H

#include <stdint.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace Coffee{
namespace CDisplay{

struct CGLFWContext
{
    GLFWwindow* window = nullptr;

    void makeCurrent(){
        glfwMakeContextCurrent(window);
    }
};

class CGLFWNativeFuncs
{
    CGLFWNativeFuncs();
public:
    static uintptr_t glfwGetNativeWindowHandle(GLFWwindow* window);
    static void glfwSetDecorations(GLFWwindow *window, bool mode);
    static void glfwSetFloating(GLFWwindow *window, bool mode);
};

}
}


#endif // CGLFWNATIVEFUNCS_H
