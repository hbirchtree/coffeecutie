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
//Mouse buttons
static void _glfw_input_mouseBtn(GLFWwindow *window,int button,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwMouseButtonEvent(button,action,mods);
}
//Keyboard keys
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwKeyboardEvent(key,action,mods);
}
//Scroll event
static void _glfw_input_scroll(GLFWwindow *window,double xoffset,double yoffset){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwMouseScrollEvent(xoffset,yoffset);
}
//Mouse enter event
static void _glfw_input_mouseenter(GLFWwindow *window,int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwMouseEnterEvent((val==GL_TRUE) ? true : false);
}
//Mouse movement
static void _glfw_input_mousemove(GLFWwindow *window, double xpos, double ypos){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwMouseMoveEvent(xpos,ypos);
}
//Drag and drop event
static void _glfw_input_dropevent(GLFWwindow *window, int numfiles,const char** paths){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QStringList files;
    for(int i=0;i<numfiles;i++)
        files << paths[i];
    rend->glfwDropEvent(files);
}
//Character writing
static void _glfw_input_charwrite(GLFWwindow *window, unsigned int character){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwTypingEvent(character);
}

//Window resize
static void _glfw_winevent_resize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->setWindowDimensionsValue(width,height);
}

/*
 * Exit signals:
 *  01 : failed to init GLFW
 *  10 : failed to create window
 *  11 : glewInit() failed
 *  12 : OpenGL 3.3 not supported
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
    glfwSetCursorEnterCallback(window,_glfw_input_mouseenter);
    glfwSetDropCallback(window,_glfw_input_dropevent);
    glfwSetScrollCallback(window,_glfw_input_scroll);
    glfwSetCharCallback(window,_glfw_input_charwrite);

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK)
        return 11;
    if(!GLEW_VERSION_3_3)
        return 12;
    glfwSwapInterval(1);

    glfwShowWindow(window);

    glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
//    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

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
    CoffeeCamera camera(this,1.6,0.1,100.0,90.0,glm::vec3(5,5,5),glm::vec3(0,0,0));
    this->camera = &camera;
    CoffeeWorldOpts world(this);
    world.setCamera(&camera);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->renderAll();
        RenderingMethods::rendering_simple(&test,&world);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    test.unloadAssets();

    return 0;
}

void CoffeeRenderer::printInput(int btn, int action){
    if(btn==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)
        requestWindowClose();
    if(btn==GLFW_MOUSE_BUTTON_1&&action==GLFW_PRESS)
        setRendererClearColor(glm::vec4(1,1,1,0));
    qDebug() << "INPUT: "+QString::number(btn)+" : "+QString::number(action);
}

void CoffeeRenderer::printInput(double x, double y){
    qDebug() << "MOVE: "+QString::number(x)+" : "+QString::number(y);
}
