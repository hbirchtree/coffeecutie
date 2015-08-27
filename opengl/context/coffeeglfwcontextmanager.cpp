#include "coffeeglfwcontextmanager.h"

#include <QMimeData>
#include "engine/scripting/coffeeinputevent.h"
#include "general/input/coffeejoystick.h"

#define RENDERER_DO_DEBUG

using namespace Coffee::CInput;

CoffeeGLFWContextManager::CoffeeGLFWContextManager(QObject *parent) :
    CoffeeRendererBase(parent)
{
}

CoffeeGLFWContextManager::~CoffeeGLFWContextManager()
{
    qDebug("Destroying GLFW window");
    if(window!=nullptr)
        glfwDestroyWindow(window);
    glfwTerminate();
}

bool CoffeeGLFWContextManager::shouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void CoffeeGLFWContextManager::swapBuffers()
{
    glfwSwapBuffers(window);
}

void CoffeeGLFWContextManager::pollEvents()
{
    glfwPollEvents();
}

void CoffeeGLFWContextManager::updateJoysticks()
{
    for(CoffeeJoystick* js : m_joysticks)
        js->update();
}

void CoffeeGLFWContextManager::setLatestFrameTime(double time)
{
    this->m_frameTime = time;
}

QWindow *CoffeeGLFWContextManager::getWindowHandle()
{
    return GLFWNativeFunctions::getQWindow(window);
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
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));

    CIMouseEvent m;
    if(button==GLFW_MOUSE_BUTTON_MIDDLE)
        m.keyCode = CIMouseEvent::MiddleButton;
    else if(button==GLFW_MOUSE_BUTTON_RIGHT)
        m.keyCode = CIMouseEvent::RightButton;
    else
        m.keyCode = button+1;
    m.modifier = mods;
    m.type = (action == GLFW_PRESS) ? CIMouseEvent::Press : CIMouseEvent::Release;

    void* d = nullptr;
    uint32_t s;

    CIEventParser::createEvent(CIEvent::Mouse,&m,sizeof(m),&d,s);
    rend->inputEventPass(d,s);
}
//Mouse movement
static void _glfw_input_mousemove(GLFWwindow *window, double xpos, double ypos){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    CIMouseEvent m;
    m.x = xpos;
    m.y = ypos;
    m.type = CIMouseEvent::Move;

    void* d = nullptr;
    uint32_t s;

    CIEventParser::createEvent(CIEvent::Mouse,&m,sizeof(m),&d,s);
    rend->inputEventPass(d,s);
}

//Mouse enter event
static void _glfw_input_mouseenter(GLFWwindow *window,int val){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    QEvent::Type t;
    if(val==1)
        t = QEvent::Enter;
    else
        t = QEvent::Leave;
    rend->winMouseEnterEvent(QEvent(t));
}

//Scroll event
static void _glfw_input_scroll(GLFWwindow *window,double xoffset,double yoffset){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    //We need to fill in some garbage. RIP memory allocation
    QWheelEvent event(QPointF(),QPointF(),QPoint(),QPoint(-xoffset*120,-yoffset*120),0,Qt::Vertical,Qt::NoButton,Qt::NoModifier);
    rend->winWheelEvent(event);
}

//Drag and drop event
static void _glfw_input_dropevent(GLFWwindow *window, int numfiles,const char** paths){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
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
    rend->rendererMessage(CoffeeGLFWContextManager::InformationMessage,"Drag-and-Drop event received");
}

//Keyboard keys
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    Q_UNUSED(scancode);
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
//    bool autorep = ((action==GLFW_REPEAT) ? true : false);
//    QEvent::Type type = ((action==GLFW_RELEASE) ? QEvent::KeyRelease : QEvent::KeyPress);

//    QKeyEvent event(type,_glfw_translate_key(key),_glfw_translate_mods(mods),QString(),autorep,1);
//    rend->winKeyboardEvent(event);
    CIKeyEvent e;
    e.type = (action == GLFW_PRESS) ? CIKeyEvent::Press : (action == GLFW_RELEASE) ? CIKeyEvent::Release : CIKeyEvent::Repeated;
    e.keyCode = _glfw_translate_key(key);
    e.modifier = mods;

    void* d = nullptr;
    uint32_t s;
    CIEventParser::createEvent(CIEvent::Keyboard,&e,sizeof(e),&d,s);
    rend->inputEventPass(d,s);
}
//Character writing
static void _glfw_input_charwrite(GLFWwindow *window, unsigned int character){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    QChar ch = QString::fromUcs4(&character).at(0);
    QKeyEvent event(QEvent::InputMethod,0,Qt::NoModifier,ch,false,1);
    rend->winKeyboardEvent(event); //If we do not do this, we get garbage along with our input.
}

//Window resize
static void _glfw_winevent_resize(GLFWwindow* window, int width, int height)
{
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    rend->winResize(QResizeEvent(QSize(width,height),rend->getWindowDimensions()));
    rend->rendererMessage(CoffeeGLFWContextManager::InformationMessage,"Window resized");
}
static void _glfw_winevent_fbresize(GLFWwindow* window, int width, int height)
{
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    rend->winFrameBufferResize(QResizeEvent(QSize(width,height),rend->getCurrentFramebufferSize()));
    rend->windowAspectChanged((float)width/(float)height);
    rend->rendererMessage(CoffeeGLFWContextManager::InformationMessage,"Framebuffer resized");
}
static void _glfw_winevent_focus(GLFWwindow* window, int val){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    //We might want to elaborate on the focus reason
    rend->winFocusChanged((val==1) ? QFocusEvent(QEvent::FocusIn,Qt::MouseFocusReason) : QFocusEvent(QEvent::FocusOut,Qt::MouseFocusReason));
}
static void _glfw_winevent_pos(GLFWwindow* window, int x,int y){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    rend->winPosChanged(QMoveEvent(QPoint(x,y),QPoint()));
}
static void _glfw_winevent_refresh(GLFWwindow* window){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    rend->winRefresh();
}
static void _glfw_winevent_close(GLFWwindow* window){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    glfwSetWindowShouldClose(window,0);
    rend->winClose();
}
static void _glfw_winevent_state(GLFWwindow* window, int val){
    CoffeeGLFWContextManager* rend =
            static_cast<CoffeeGLFWContextManager*>(glfwGetWindowUserPointer(window));
    //Not very accurate
    rend->windowStateChanged((val==1) ? QWindowStateChangeEvent(Qt::WindowMinimized) : QWindowStateChangeEvent(Qt::WindowMaximized));
    rend->rendererMessage(CoffeeGLFWContextManager::InformationMessage,"Window state changed");
}

static void _glfw_error_function(int stat, const char* message){
    qDebug("GLFW error message: error %i: %s",stat,message);
}

void CoffeeGLFWContextManager::setWindowTitle(const QString &value)
{
    CoffeeRendererBase::setWindowTitle(value);
    std::string _t = value.toStdString();
    if(window)
	glfwSetWindowTitle(window,_t.c_str());
}

void CoffeeGLFWContextManager::queueFunction(std::function<void()> func)
{
    CoffeeFunctionQueueRunner::queueFunction(func);
}

void CoffeeGLFWContextManager::setWindowDimensions(const QSize &windowSize)
{
    CoffeeRendererBase::setWindowDimensions(windowSize);
    if(window)
        glfwSetWindowSize(window,windowSize.width(),windowSize.height());
}

GLFWwindow* CoffeeGLFWContextManager::setWindowedFullscreen(int monitor, const char *title)
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

    return glfwCreateWindow(current->width,current->height,
			    title,mon,NULL);
}

GLFWwindow* CoffeeGLFWContextManager::setFullscreen(int monitor, const char *title)
{
    int count;
    GLFWmonitor** data = glfwGetMonitors(&count);
    if(monitor>=count)
        return NULL;

    GLFWmonitor* mon = data[monitor];

    return glfwCreateWindow(getWindowDimensions().width(),getWindowDimensions().height(),
			    title,mon,NULL);
}

GLFWwindow* CoffeeGLFWContextManager::setWindowed(const char* title)
{
    return glfwCreateWindow(getWindowDimensions().width(),getWindowDimensions().height(),
			    title,NULL,NULL);
}

void CoffeeGLFWContextManager::setMousePosition(QPointF pos)
{
    if(window){
        glfwSetCursorPos(window,pos.x(),pos.y());
    }
}

void CoffeeGLFWContextManager::setMouseGrabbing(bool state)
{
    if(window){
        uint32_t mode;
        if(state){
            mode = GLFW_CURSOR_DISABLED;
        }else{
            mode = GLFW_CURSOR_NORMAL;
        }
        queueFunction([=](){
            glfwSetInputMode(window,GLFW_CURSOR,mode);
        });
    }
}

void CoffeeGLFWContextManager::requestWindowClose(){
    if(window){
        glfwSetWindowShouldClose(window,1);
    }
}

void CoffeeGLFWContextManager::showWindow()
{
    glfwShowWindow(window);
}

void CoffeeGLFWContextManager::hideWindow()
{
    glfwHideWindow(window);
}

void CoffeeGLFWContextManager::setSwapInterval(uint interval)
{
    queueFunction([=](){
        glfwSwapInterval(interval);
    });
}

double CoffeeGLFWContextManager::getLoopTime() const
{
    return glfwGetTime();
}

bool CoffeeGLFWContextManager::isMouseGrabbed() const
{
    if(window){
        return glfwGetInputMode(window,GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    }
    return false;
}

double CoffeeGLFWContextManager::getLatestFrameTime() const
{
    return m_frameTime;
}

QWindow *CoffeeGLFWContextManager::windowHandle()
{
    return GLFWNativeFunctions::getQWindow(this->window);
}

void CoffeeGLFWContextManager::setWindowState(Qt::WindowState state)
{
    CoffeeRendererBase::setWindowState(state);
    if(window){
        switch(state){
        case Qt::WindowMinimized:
            glfwIconifyWindow(window);
            break;
        case Qt::WindowMaximized:
            glfwShowWindow(window);
            break;
        case Qt::WindowFullScreen:
            // TODO : Implement this!
            break;
        default:
            break;
        }
    }
}

int CoffeeGLFWContextManager::init(){

    rendererMessage(InformationMessage,"Setting up GLFW context");

    connect(this,&CoffeeGLFWContextManager::winFrameBufferResize,
            [=](QResizeEvent ev){
        glViewport(0,0,ev.size().width(),ev.size().height());
        setFramebufferSize(ev.size());
    });

    if(!glfwInit()){
        qFatal("Failed to initialize GLFW!");
    }
    rendererMessage(InformationMessage,
                          "GLFW initialized");

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_SAMPLES,m_multisamples);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef RENDERER_DO_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,true);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,false);
    glfwWindowHint(GLFW_RESIZABLE,true);

    glfwSetErrorCallback(_glfw_error_function);

    std::string _title_std = getWindowTitle().toStdString();
    const char* title = _title_std.c_str();
    switch(windowState()){
    case Qt::WindowFullScreen:{
        rendererMessage(InformationMessage,
                              "Setting exclusive fullscreen mode");
	window = setFullscreen(startDisplay(),title);
        break;
    }
    case Qt::WindowMaximized:{
        rendererMessage(InformationMessage,
                              "Setting windowed fullscreen mode");
	window = setWindowedFullscreen(startDisplay(),title);
        break;
    }
    default:{
        rendererMessage(InformationMessage,
                              "Setting windowed mode");
	window = setWindowed(title);
        break;
    }
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
        m_contextManager = QString("GLFW %1.%2 rev. %3")
                .arg(maj)
                .arg(min)
                .arg(rev);
    }

    rendererMessage(InformationMessage,
                          "Initializing glbinding");
    glbinding::Binding::initialize(true);

    m_renderer = QString::fromStdString(glbinding::ContextInfo::renderer());
    m_vendor = QString::fromStdString(glbinding::ContextInfo::vendor());
    m_version = QString::fromStdString(glbinding::ContextInfo::version().toString());

#ifdef RENDERER_DO_DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    if(glDebugMessageCallback){
        glDebugMessageCallback(openGLDebugCallback,this);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
                              GL_DONT_CARE,
                              GL_DONT_CARE,
                              0,
                              &unusedIds,
                              GL_TRUE);
        rendererMessage(InformationMessage,
                              "OpenGL debugging is enabled");
    }else
        rendererMessage(InformationMessage,
                              "OpenGL debugging not available!");
#endif

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return 0;
}

QObject *CoffeeGLFWContextManager::getJoystickDevice(uint index)
{
    if(glfwJoystickPresent(index)){
        CoffeeJoystick* js = new CoffeeGLFWJoystick(this,index);
        m_joysticks.append(js);
        rendererMessage(InformationMessage,
                              QString("Joystick device created: %1")
                              .arg(js->getJoystickName()));
        return js;
    }else{
        return nullptr;
    }
}

QString CoffeeGLFWContextManager::renderer() const
{
    return m_renderer;
}

QString CoffeeGLFWContextManager::vendor() const
{
    return m_vendor;
}

QString CoffeeGLFWContextManager::version() const
{
    return m_version;
}

QString CoffeeGLFWContextManager::contextManager() const
{
    return m_contextManager;
}
