#include "coffeerenderer.h"
#include "components/coffeeobject.h"
#include "general/models/wavefrontmodelreader.h"
#include "helpers/texturehelper.h"
#include "opengl/helpers/renderingmethods.h"

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h) : QThread(parent)
{
    CoffeeRenderer(w,h);
}
CoffeeRenderer::CoffeeRenderer(int w, int h) : QThread()
{
    setWindowDimensions(w,h);
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

void CoffeeRenderer::setWindowDimensions(int w, int h){
    width = w;
    height = h;
    updateWindowDimensions(w,h);
}
void CoffeeRenderer::setWindowDimensionsValue(int w, int h){
    width = w;
    height = h;
}
void CoffeeRenderer::setWindowTitle(QString title){
    windowTitle = title;
    updateWindowTitle(title);
}
void CoffeeRenderer::setRendererClearColor(glm::vec4 col)
{
    clearColor = col;
    updateRendererClearColor(clearColor);
}

void CoffeeRenderer::updateWindowTitle(QString value){
    glfwSetWindowTitle(window,value.toStdString().c_str());
}
void CoffeeRenderer::updateRendererClearColor(glm::vec4 value){
    glClearColor(value.r,value.g,value.b,value.a);
}
void CoffeeRenderer::updateWindowDimensions(int w,int h){
    glfwSetWindowSize(window,w,h);
}
void CoffeeRenderer::requestWindowClose(){
    glfwSetWindowShouldClose(window,GL_TRUE);
}

static void errorCallback(int error, const char* description){
    QString errorMessage = "ERROR: "+QString::number(error)+" : "+QString::fromLocal8Bit(description);
    printf("%s\n",errorMessage.toStdString().c_str());
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
//    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
//    rend->printInput(xpos,ypos);
}
static void _glfw_winevent_resize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->setWindowDimensionsValue(width,height);
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

    glfwWindowHint(GLFW_SAMPLES,samples);
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
    glfwSetWindowSizeCallback(window,_glfw_winevent_resize);
    glfwSetCursorPosCallback(window,_glfw_input_mousemove);

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSwapInterval(1);

    glfwShowWindow(window);

    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

int CoffeeRenderer::loop(){
    WavefrontModelReader rdr(this);
    QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > mdls = rdr.parseModel("testgame/models/3dbox_bumped.obj");
    CoffeeObject test(this);
    test.setShader(new ShaderContainer(this));
    test.getShader()->buildProgram("testgame/shaders/vsh.txt","testgame/shaders/fsh.txt");
    QList<QPointer<WavefrontModelReader::ModelContainer> > vals = mdls.values();
    test.setModel(vals.first()->model);
    test.setMaterial(vals.first()->material);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->renderAll();
        RenderingMethods::rendering_simple(&test);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
