#include "coffeerenderer.h"

#include <QCoreApplication>
#include "opengl/rendering/renderloop.h"
#include <QMimeData>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QDropEvent>
#include "engine/scripting/coffeeinputevent.h"

#define RENDERER_DO_DEBUG

CoffeeRenderer::CoffeeRenderer(QObject *parent) : QObject(parent)
{
    connect(this,&CoffeeRenderer::winMouseEnterEvent,[=](QEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winMouseEvent,[=](QMouseEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winMouseGrabbed,[=](QEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });
    connect(this,&CoffeeRenderer::winWheelEvent,[=](QWheelEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });

    connect(this,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent e){
        emit inputEvent(new CoffeeInputEvent(0,e));
    });

//    connect(this,&CoffeeRenderer::winDropEvent,ih);
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h) : CoffeeRenderer(parent)
{
    setWindowDimensions(QSize(w,h));
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state) : CoffeeRenderer(parent,w,h)
{
    startmode = state;
}

CoffeeRenderer::CoffeeRenderer(QObject *parent, int w, int h, Qt::WindowState state, QString windowTitle) : CoffeeRenderer(parent,w,h,state)
{
    this->windowTitle = windowTitle;
}

CoffeeRenderer::~CoffeeRenderer()
{
    qDebug("Destroying GLFW window");
    if(window!=NULL)
        glfwDestroyWindow(window);
    glfwTerminate();
}

void CoffeeRenderer::updateWindowTitle(QString value){
    windowTitle = value;
    glfwSetWindowTitle(window,value.toStdString().c_str());
}
void CoffeeRenderer::updateRendererClearColor(glm::vec4 value){
    glClearColor(value.r,value.g,value.b,value.a);
}
void CoffeeRenderer::updateWindowDimensions(QSize dims){
    windowDimensions.setWidth(dims.width());
    windowDimensions.setHeight(dims.height());
    glfwSetWindowSize(window,dims.width(),dims.height());
}

void CoffeeRenderer::setWindowState(Qt::WindowState state)
{
    switch(state){
    case Qt::WindowMinimized:
        glfwIconifyWindow(window);
        break;
    case Qt::WindowMaximized:
        glfwRestoreWindow(window);
        break;
    default:
        break;
    }
}

GLFWwindow* CoffeeRenderer::setWindowedFullscreen(int monitor)
{
    int count;
    GLFWmonitor** data = glfwGetMonitors(&count);
    if(monitor>=count)
        return NULL;

    GLFWmonitor* mon = data[monitor];
    const GLFWvidmode* current = glfwGetVideoMode(mon);

    glfwWindowHint(GLFW_RED_BITS,current->redBits);
    glfwWindowHint(GLFW_GREEN_BITS,current->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS,current->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE,current->refreshRate);

    qDebug("Setting windowed fullscreen mode: %ix%i, title: %s, monitor: %i",
           windowDimensions.width(),windowDimensions.height(),
           windowTitle.toStdString().c_str(),monitor);

    return glfwCreateWindow(current->width,current->height,
                            windowTitle.toStdString().c_str(),mon,NULL);
}

GLFWwindow* CoffeeRenderer::setFullscreen(int monitor)
{
    int count;
    GLFWmonitor** data = glfwGetMonitors(&count);
    if(monitor>=count)
        return NULL;

    GLFWmonitor* mon = data[monitor];

    qDebug("Setting fullscreen mode: %ix%i, title: %s, monitor: %i",
           windowDimensions.width(),windowDimensions.height(),
           windowTitle.toStdString().c_str(),monitor);

    return glfwCreateWindow(windowDimensions.width(),windowDimensions.height(),
                            windowTitle.toStdString().c_str(),mon,NULL);
}

GLFWwindow *CoffeeRenderer::setWindowed()
{
    qDebug("Setting windowed mode: %ix%i, title: %s",
           windowDimensions.width(),windowDimensions.height(),
           windowTitle.toStdString().c_str());
    return glfwCreateWindow(windowDimensions.width(),windowDimensions.height(),
                            windowTitle.toStdString().c_str(),NULL,NULL);
}
int CoffeeRenderer::getSamples() const
{
    return samples;
}

void CoffeeRenderer::getVideoMemoryUsage(qint32 *current, qint32 *total) const
{
    if(this->vmem_free==0&&vmem_total==0)
        return;
    if(!current||!total)
        return;
    *current = vmem_free;
    *total = vmem_total;
}

void CoffeeRenderer::queueFunction(std::function<void ()> *func)
{
    runqueue.append(func);
}

void CoffeeRenderer::setSwapInterval(uint interval)
{
    glfwSwapInterval(interval);
}

void CoffeeRenderer::setSamples(uint value)
{
    samples = value;
}

void CoffeeRenderer::setLoop(RenderLoop *value)
{
    loopObject = value;
}

QSize* CoffeeRenderer::getFramebufferSizePt()
{
    return &framebufferSize;
}

int CoffeeRenderer::getMouseInputMode() const
{
    if(!window)
        return 0;
    return glfwGetInputMode(window,0);
}

bool CoffeeRenderer::isMouseGrabbed() const
{
    if(!window)
        return false;
    int mode = glfwGetInputMode(window,GLFW_CURSOR);
    return (mode == GLFW_CURSOR_DISABLED);
}

double CoffeeRenderer::getLatestFrameTime() const
{
    return frametime;
}

QSize CoffeeRenderer::getWindowDimensions() const
{
    return windowDimensions;
}

void CoffeeRenderer::setWindowDimensions(const QSize &value)
{
    windowDimensions = value;
}

QSize CoffeeRenderer::getCurrentFramebufferSize() const
{
    if(!window)
        return QSize();
    int width,height;
    glfwGetFramebufferSize(window, &width, &height);
    return QSize(width,height);
}

int CoffeeRenderer::getStartDisplay() const
{
    return startDisplay;
}

double CoffeeRenderer::getLoopTime() const
{
    if(!window)
        return 0.0;
    return glfwGetTime();
}

QString CoffeeRenderer::getWindowTitle() const
{
    return windowTitle;
}

void CoffeeRenderer::setStartDisplay(uint value)
{
    startDisplay = value;
}

void CoffeeRenderer::setStartmode(const Qt::WindowState &value)
{
    startmode = value;
}

void CoffeeRenderer::updateMouseGrabbing(bool state)
{
    if(state){
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        emit winMouseGrabbed(QEvent(QEvent::GrabMouse));
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        emit winMouseGrabbed(QEvent(QEvent::UngrabMouse));
    }
}

void CoffeeRenderer::setMousePos(int x, int y)
{
    glfwSetCursorPos(window,x,y);
}

void CoffeeRenderer::requestWindowClose(){
    glfwSetWindowShouldClose(window,1);
}

static Qt::KeyboardModifiers _glfw_translate_mods(int mods){
    Qt::KeyboardModifiers qmods;
    if(mods & GLFW_MOD_ALT)
        qmods|=Qt::AltModifier;
    if(mods & GLFW_MOD_SHIFT)
        qmods|=Qt::ShiftModifier;
    if(mods & GLFW_MOD_CONTROL)
        qmods|=Qt::ControlModifier;
    if(mods & GLFW_MOD_SUPER)
        qmods|=Qt::MetaModifier;
    return qmods;
}

static int _glfw_translate_key(int key){
    if((key>=GLFW_KEY_A&&key<=GLFW_KEY_Z)||(key>=GLFW_KEY_0&&key<=GLFW_KEY_9))
        return key; //same mapping between Qt::Key and GLFW
    switch(key){
    case GLFW_KEY_ENTER:{
        return Qt::Key_Enter;
    }
    case GLFW_KEY_ESCAPE:{
        return Qt::Key_Escape;
    }
    }
    return Qt::Key_unknown;
}

//Mouse buttons
static void _glfw_input_mouseBtn(GLFWwindow *window,int button,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QEvent::Type type = ((action==GLFW_PRESS) ? QMouseEvent::MouseButtonPress : QMouseEvent::MouseButtonRelease);
    Qt::MouseButton btn = Qt::NoButton;
    switch(button){
    case GLFW_MOUSE_BUTTON_1:
        btn = Qt::LeftButton;
        break;
    case GLFW_MOUSE_BUTTON_2:
        btn = Qt::RightButton;
        break;
    case GLFW_MOUSE_BUTTON_3:
        btn = Qt::MiddleButton;
        break;
    case GLFW_MOUSE_BUTTON_4:
        btn = Qt::XButton1;
        break;
    case GLFW_MOUSE_BUTTON_5:
        btn = Qt::XButton2;
        break;
    default:
        qDebug() << "Unhandled mouse button: " << button;
    }
    QMouseEvent event(type,QPoint(),btn,Qt::NoButton,_glfw_translate_mods(mods));
    rend->winMouseEvent(event);
}
//Mouse movement
static void _glfw_input_mousemove(GLFWwindow *window, double xpos, double ypos){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QMouseEvent event(QEvent::MouseMove,QPointF(xpos,ypos),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
    rend->winMouseEvent(event);
}

//Mouse enter event
static void _glfw_input_mouseenter(GLFWwindow *window,int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QEvent::Type t;
    if(val==1)
        t = QEvent::Enter;
    else
        t = QEvent::Leave;
    rend->winMouseEnterEvent(QEvent(t));
}

//Scroll event
static void _glfw_input_scroll(GLFWwindow *window,double xoffset,double yoffset){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //We need to fill in some garbage. RIP memory allocation
    QWheelEvent event(QPointF(),QPointF(),QPoint(),QPoint(-xoffset*120,-yoffset*120),0,Qt::Vertical,Qt::NoButton,Qt::NoModifier);
    rend->winWheelEvent(event);
}

//Drag and drop event
static void _glfw_input_dropevent(GLFWwindow *window, int numfiles,const char** paths){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QMimeData *data = new QMimeData();
    if(numfiles>0&&QUrl(paths[0]).isValid()){
        QList<QUrl> files;
        for(int i=0;i<numfiles;i++){
            files.append(QUrl(paths[i]));
        }
        data->setUrls(files);
    }else{
        for(int i=0;i<numfiles;i++)
            data->setText(data->text()+paths[i]);
    }
    QDropEvent ev(QPoint(),Qt::LinkAction,data,Qt::NoButton,Qt::NoModifier);
    rend->winDropEvent(ev);
}

//Keyboard keys
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    Q_UNUSED(scancode);
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    bool autorep = ((action==GLFW_REPEAT) ? true : false);
    QEvent::Type type = ((action==GLFW_RELEASE) ? QEvent::KeyRelease : QEvent::KeyPress);

    //TODO : Translate int key to Qt::Key! We'll just stick the GLFW_KEY in there for now. Translation might be too slow anyway.

    QKeyEvent event(type,_glfw_translate_key(key),_glfw_translate_mods(mods),QString(),autorep,1);
    rend->winKeyboardEvent(event);
}
//Character writing
static void _glfw_input_charwrite(GLFWwindow *window, unsigned int character){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QChar ch = QString::fromUcs4(&character).at(0);
    QKeyEvent event(QEvent::InputMethod,0,Qt::NoModifier,ch,false,1);
    rend->winKeyboardEvent(event); //If we do not do this, we get garbage along with our input.
}

//Window resize
static void _glfw_winevent_resize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->winResize(QResizeEvent(QSize(width,height),rend->getWindowDimensions()));
}
static void _glfw_winevent_fbresize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->winFrameBufferResize(QResizeEvent(QSize(width,height),rend->getCurrentFramebufferSize()));
}
static void _glfw_winevent_focus(GLFWwindow* window, int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //We might want to elaborate on the focus reason
    rend->winFocusChanged((val==1) ? QFocusEvent(QEvent::FocusIn,Qt::MouseFocusReason) : QFocusEvent(QEvent::FocusOut,Qt::MouseFocusReason));
}
static void _glfw_winevent_pos(GLFWwindow* window, int x,int y){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->winPosChanged(QMoveEvent(QPoint(x,y),QPoint()));
}
static void _glfw_winevent_refresh(GLFWwindow* window){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->winRefresh();
}
static void _glfw_winevent_close(GLFWwindow* window){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    glfwSetWindowShouldClose(window,0);
    rend->winClose();
}
static void _glfw_winevent_state(GLFWwindow* window, int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //Not very accurate
    rend->winStateChanged((val==1) ? QWindowStateChangeEvent(Qt::WindowMinimized) : QWindowStateChangeEvent(Qt::WindowMaximized));
}

static void _glfw_error_function(int stat, const char* message){
    qDebug("GLFW error message: error %i: %s",stat,message);
}

/*
 * Exit signals:
 *  01 : failed to init GLFW
 *  10 : failed to create window
 *  11 : glewInit() failed
 *  12 : OpenGL 3.3 not supported
 */


int CoffeeRenderer::init(){
    connect(this,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        glViewport(0,0,ev.size().width(),ev.size().height());
        framebufferSize = ev.size();
    });

    if(!glfwInit()){
        qFatal("Failed to initialize GLFW!");
    }

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_SAMPLES,samples);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,true);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef RENDERER_DO_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,true);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,false);
    glfwWindowHint(GLFW_RESIZABLE,true);

    glfwSetErrorCallback(_glfw_error_function);

    switch(startmode){
    case Qt::WindowFullScreen:
        window = setFullscreen(startDisplay);
        break;
    case Qt::WindowMaximized:
        window = setWindowedFullscreen(startDisplay);
        break;
    default:
        window = setWindowed();
        break;
    }
    if(window==NULL){
        qFatal("Failed to create window");
        return 10;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window,this);

    //Input callbacks
    glfwSetMouseButtonCallback(window,_glfw_input_mouseBtn);
    glfwSetKeyCallback(window,_glfw_input_kbdKey);
    glfwSetCursorPosCallback(window,_glfw_input_mousemove);
    glfwSetCursorEnterCallback(window,_glfw_input_mouseenter);
    glfwSetDropCallback(window,_glfw_input_dropevent);
    glfwSetScrollCallback(window,_glfw_input_scroll);
    glfwSetCharCallback(window,_glfw_input_charwrite);

    glfwSetWindowSizeCallback(window,_glfw_winevent_resize);
    glfwSetWindowCloseCallback(window,_glfw_winevent_close);
    glfwSetWindowFocusCallback(window,_glfw_winevent_focus);
    glfwSetWindowIconifyCallback(window,_glfw_winevent_state);
    glfwSetWindowPosCallback(window,_glfw_winevent_pos);
    glfwSetWindowRefreshCallback(window,_glfw_winevent_refresh);
    glfwSetFramebufferSizeCallback(window,_glfw_winevent_fbresize);

    {
        int maj,min,rev;
        glfwGetVersion(&maj,&min,&rev);
        qDebug("GLFW version: %i.%i rev. %i",maj,min,rev);
    }

    glbinding::Binding::initialize(true);
    qDebug("%s",QString("\nOpenGL renderer: %1\nOpenGL vendor  : %2\nOpenGL version : %3")
           .arg(QString::fromStdString(glbinding::ContextInfo::renderer()))
           .arg(QString::fromStdString(glbinding::ContextInfo::vendor()))
           .arg(QString::fromStdString(glbinding::ContextInfo::version().toString())).toStdString().c_str());

    setSwapInterval(1);

//    glbinding::setAfterCallback([](const glbinding::FunctionCall&){
//        gl::GLenum error = glGetError();
//        if(error!=GL_NO_ERROR)
//            fprintf(stderr,"OGLERROR: error code:%i\n",static_cast<int>(error));
//    });
//    glbinding::setBeforeCallback([](const glbinding::FunctionCall&){
//        gl::GLenum error = glGetError();
//        if(error!=GL_NO_ERROR)
//            fprintf(stderr,"OGLERROR: error code:%i\n",static_cast<int>(error));
//    });

#ifdef RENDERER_DO_DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    if(glDebugMessageCallback){
        glDebugMessageCallback(openGLDebugCallback,nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
                              GL_DONT_CARE,
                              GL_DONT_CARE,
                              0,
                              &unusedIds,
                              GL_TRUE);
    }else
        qDebug() << "No debug 4 u!";
#endif

    glfwShowWindow(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return 0;
}

/*
 * 01 : no rendering process specified
 */
int CoffeeRenderer::loop(){
    if(!loopObject){
        qFatal("No loop object defined!");
    }
    qDebug("Initializing loop");

    std::function<void()> *_init = loopObject->getInit();
    std::function<void()> *_loop = loopObject->getLoop();
    std::function<void()> *_cleanup = loopObject->getCleanup();

    double frametime = 0.0;

    qDebug("Running initialization function");
    (*_init)();

    {
        emit winFrameBufferResize(QResizeEvent(getWindowDimensions(),getWindowDimensions()));
    }

    qDebug("Running loop function");
    while(!glfwWindowShouldClose(window)){
        if(gpumemcheck){
            if(gpumemcheck_nvidia){
                glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,&vmem_total);
                glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,&vmem_free);

                if(vmem_total==0||vmem_free==0){ //If check fails, stop checking.
                    gpumemcheck_nvidia = false;
                }
            }
            //We will see if we can add Intel and AMD support later.
            //So far, it seems like AMD only support Windows with that.
            //I will never implement Windows-only functionality.
        }

        frametime = glfwGetTime();
        glfwPollEvents();
        (*_loop)();
        if(runqueue.size()>0){
            QVector<std::function<void()>*> t_queue(runqueue);
            for(auto f : t_queue){
                (*f)();
                delete f;
                runqueue.removeOne(f);
            }
        }
        glfwSwapBuffers(window);
        this->frametime = glfwGetTime()-frametime;
        contextReportFrametime(this->frametime);
    }
    qDebug("Running cleanup function");
    (*_cleanup)();

    qDebug("Estimated uptime: %.1f seconds",glfwGetTime());
    return 0;
}

void CoffeeRenderer::openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Q_UNUSED(id)
    Q_UNUSED(userParam)
    if(severity==GL_DEBUG_SEVERITY_NOTIFICATION)
        return;
    QByteArray messageB;
    messageB.setRawData(message,length);
    qDebug("OpenGL debug callback:\n%s(%s):%s:\n%s",
           glbinding::Meta::getString(source).c_str(),
           glbinding::Meta::getString(type).c_str(),
           glbinding::Meta::getString(severity).c_str(),
           messageB.toStdString().c_str());
}

void CoffeeRenderer::run()
{
    int stat = init();
    if(stat!=0)
        qFatal("Failed to initialize renderer!");
    stat = loop();
    if(stat!=0)
        qFatal("Loop function failed!");

    qDebug("Stopping rendering thread");
}

void CoffeeRenderer::flushPipeline()
{
    glFlush();
}

void CoffeeRenderer::requestMemoryCheck()
{
    gpumemcheck = true;
}
