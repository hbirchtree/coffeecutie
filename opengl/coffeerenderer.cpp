#include "coffeerenderer.h"

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h) : QThread(parent)
{
    CoffeeRenderer(w,h);
}
CoffeeRenderer::CoffeeRenderer(int w, int h) : QThread()
{
    width = w;
    height = h;
    clearColor.r = 0.0;
    clearColor.g = 0.0;
    clearColor.b = 0.0;
    clearColor.a = 1.0;
}

CoffeeRenderer::~CoffeeRenderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void errorCallback(int error, const char* description){
    QString errorMessage = "ERROR: "+QString::number(error)+" : "+QString::fromLocal8Bit(description);
    printf(errorMessage.toStdString().c_str());
}
static void _glfw_input_mouseBtn(GLFWwindow *window,int button,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->printInput(button,action);
}
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->printInput(key,action);
}
static void _glfw_input_mousemove(GLFWwindow *window, double xpos, double ypos){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->printInput(xpos,ypos);
}

/*
 * Exit signals:
 *  1 : failed to init GLFW
 *  10 : failed to create window
 *
 */
int CoffeeRenderer::init(){
    if(!glfwInit())
        return 1;

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_SAMPLES,2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE,GL_TRUE);

    window = glfwCreateWindow(width,height,windowTitle.toStdString().c_str(),NULL,NULL);
    if(window==NULL)
        return 10;

    glfwSetWindowUserPointer(window,this);

    //Input callbacks
    glfwSetMouseButtonCallback(window,_glfw_input_mouseBtn);
    glfwSetKeyCallback(window,_glfw_input_kbdKey);
//    glfwSetCursorPosCallback(window,_glfw_input_mousemove);

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    glfwShowWindow(window);

    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
    return 0;
}

int CoffeeRenderer::loop(){

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->renderAll();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
