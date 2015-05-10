#include "coffeerenderer.h"

CoffeeRenderer::CoffeeRenderer(QObject *parent) : QObject(parent)
{
    connect(this,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent event){
        framebufferSize = event.size();
    });
    connect(this,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        glViewport(0,0,ev.size().width(),ev.size().height());
    });
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

GLFWwindow* CoffeeRenderer::setWindowedFullscreen(uint monitor)
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

GLFWwindow* CoffeeRenderer::setFullscreen(uint monitor)
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
    return glfwGetInputMode(window,0);
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
    return glfwGetTime();
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

static void errorCallback(int error, const char* description){
    QString errorMessage = "GLFW:"+QString::number(error)+": "+QString::fromLocal8Bit(description);
    fprintf(stderr,"%s\n",errorMessage.toStdString().c_str());
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
    QPointer<QMimeData> data = new QMimeData();
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
    rend->winDropEvent(data);
}

//Keyboard keys
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    Q_UNUSED(scancode);
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    bool autorep = ((action==GLFW_REPEAT) ? true : false);
    QEvent::Type type = ((action==GLFW_RELEASE) ? QEvent::KeyRelease : QEvent::KeyPress);

    //TODO : Translate int key to Qt::Key! We'll just stick the GLFW_KEY in there for now. Translation might be too slow anyway.

    QKeyEvent event(type,key,_glfw_translate_mods(mods),QString(),autorep,1);
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
    rend->winResize(QResizeEvent(QSize(width,height),QSize()));
}
static void _glfw_winevent_fbresize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->winFrameBufferResize(QResizeEvent(QSize(width,height),QSize()));
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

/*
 * Exit signals:
 *  01 : failed to init GLFW
 *  10 : failed to create window
 *  11 : glewInit() failed
 *  12 : OpenGL 3.3 not supported
 */


int CoffeeRenderer::init(){
    if(!glfwInit()){
        qFatal("Failed to initialize GLFW!");
    }

    glfwSetErrorCallback(errorCallback);

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_SAMPLES,samples);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,true);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,false);
    glfwWindowHint(GLFW_RESIZABLE,true);


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

    glbinding::setAfterCallback([](const glbinding::FunctionCall&){
        gl::GLenum error = glGetError();
        if(error!=GL_NO_ERROR)
            fprintf(stderr,"OGLERROR: error code:%i\n",static_cast<int>(error));
    });

    glfwShowWindow(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return 0;
}

/*
 * 01 : no rendering process specified
 */
int CoffeeRenderer::loop(){
    if(!loopObject){
        qDebug("No loop object defined!");
        return 1;
    }
    qDebug("Initializing loop");

    std::function<void()> *_init = loopObject->getInit();
    std::function<void()> *_loop = loopObject->getLoop();
    std::function<void()> *_cleanup = loopObject->getCleanup();

    double framerate = glfwGetTime();

    qDebug("Running initialization function");
    (*_init)();
    qDebug("Running loop function");
    while(!glfwWindowShouldClose(window)){
        framerate = glfwGetTime();
        glfwPollEvents();
//        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        (*_loop)();
        glfwSwapBuffers(window);
        contextReportFrametime(glfwGetTime()-framerate);
    }
    qDebug("Running cleanup function");
    (*_cleanup)();

    qDebug("Estimated uptime: %.1f seconds",glfwGetTime());
    return 0;
}
