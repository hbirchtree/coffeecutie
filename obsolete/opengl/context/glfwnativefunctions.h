#ifndef GLFWNATIVEFUNCTIONS_H
#define GLFWNATIVEFUNCTIONS_H

#include <QWindow>

class GLFWwindow;

class GLFWNativeFunctions
{
public:
    static QWindow *getQWindow(GLFWwindow *window);
};

#endif // GLFWNATIVEFUNCTIONS_H
