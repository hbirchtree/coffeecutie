#include "coffeerenderer.h"

CoffeeRenderer::CoffeeRenderer(QObject *parent) : QThread(parent)
{
    clearColor.r = 0.0;
    clearColor.g = 0.2;
    clearColor.b = 0.2;
    clearColor.a = 1.0;

    connect(this,&CoffeeRenderer::glfwKeyboardEvent,[=](QKeyEvent event){
        if(event.key()==GLFW_KEY_ESCAPE&&event.type()==QEvent::KeyPress)
            requestWindowClose();
    });
    connect(this,&CoffeeRenderer::glfwWinClose,[=](){
        requestWindowClose();
    });
    connect(this,&CoffeeRenderer::glfwFrameBufferResize,[=](QResizeEvent event){
        framebufferSize = event.size();
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
    if(window!=NULL)
        glfwDestroyWindow(window);
    glfwTerminate();
}

void CoffeeRenderer::updateWindowTitle(QString value){
    windowTitle = value;
    glfwSetWindowTitle(window,value.toStdString().c_str());
}
void CoffeeRenderer::updateRendererClearColor(glm::vec4 value){
    clearColor = value;
    glClearColor(value.r,value.g,value.b,value.a);
}
void CoffeeRenderer::updateWindowDimensions(int w,int h){
    windowDimensions.setWidth(w);
    windowDimensions.setHeight(h);
    glfwSetWindowSize(window,w,h);
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

    return glfwCreateWindow(current->width,current->height,windowTitle.toStdString().c_str(),mon,NULL);
}

GLFWwindow* CoffeeRenderer::setFullscreen(int monitor)
{
    int count;
    GLFWmonitor** data = glfwGetMonitors(&count);
    if(monitor>=count)
        return NULL;

    GLFWmonitor* mon = data[monitor];

    return glfwCreateWindow(windowDimensions.width(),windowDimensions.height(),windowTitle.toStdString().c_str(),mon,NULL);
}

GLFWwindow *CoffeeRenderer::setWindowed()
{
    return glfwCreateWindow(windowDimensions.width(),windowDimensions.height(),windowTitle.toStdString().c_str(),NULL,NULL);
}
QSize* CoffeeRenderer::getFramebufferSizePt()
{
    return &framebufferSize;
}

QSize CoffeeRenderer::getWindowDimensions() const
{
    return windowDimensions;
}

void CoffeeRenderer::setWindowDimensions(const QSize &value)
{
    windowDimensions = value;
}

int CoffeeRenderer::getStartDisplay() const
{
    return startDisplay;
}

void CoffeeRenderer::setStartDisplay(int value)
{
    startDisplay = value;
}

void CoffeeRenderer::setStartmode(const Qt::WindowState &value)
{
    startmode = value;
}

void CoffeeRenderer::requestWindowClose(){
    glfwSetWindowShouldClose(window,1);
}

static void errorCallback(int error, const char* description){
    QString errorMessage = "ERROR: "+QString::number(error)+" : "+QString::fromLocal8Bit(description);
    printf("%s\n",errorMessage.toStdString().c_str());
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
    rend->glfwMouseEvent(event);
}
//Mouse movement
static void _glfw_input_mousemove(GLFWwindow *window, double xpos, double ypos){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QMouseEvent event(QEvent::MouseMove,QPointF(xpos,ypos),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
    rend->glfwMouseEvent(event);
}

//Mouse enter event
static void _glfw_input_mouseenter(GLFWwindow *window,int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QEvent::Type t;
    if(val==1)
        t = QEvent::Enter;
    else
        t = QEvent::Leave;
    rend->glfwMouseEnterEvent(QEvent(t));
}

//Scroll event
static void _glfw_input_scroll(GLFWwindow *window,double xoffset,double yoffset){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //We need to fill in some garbage. RIP memory allocation
    QWheelEvent event(QPointF(),QPointF(),QPoint(),QPoint(-xoffset*120,-yoffset*120),0,Qt::Vertical,Qt::NoButton,Qt::NoModifier);
    rend->glfwWheelEvent(event);
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
    rend->glfwDropEvent(data);
}

//Keyboard keys
static void _glfw_input_kbdKey(GLFWwindow *window,int key,int scancode,int action,int mods){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    bool autorep = ((action==GLFW_REPEAT) ? true : false);
    QEvent::Type type = ((action==GLFW_RELEASE) ? QEvent::KeyRelease : QEvent::KeyPress);

    //TODO : Translate int key to Qt::Key! We'll just stick the GLFW_KEY in there for now. Translation might be too slow anyway.

    QKeyEvent event(type,key,_glfw_translate_mods(mods),QString(),autorep,1);
    rend->glfwKeyboardEvent(event);
}
//Character writing
static void _glfw_input_charwrite(GLFWwindow *window, unsigned int character){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    QChar ch = QString::fromUcs4(&character).at(0);
    QKeyEvent event(QEvent::InputMethod,0,Qt::NoModifier,ch,false,1);
    rend->glfwKeyboardEvent(event); //If we do not do this, we get garbage along with our input.
}

//Window resize
static void _glfw_winevent_resize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwWinResize(QResizeEvent(QSize(width,height),QSize()));
}
static void _glfw_winevent_fbresize(GLFWwindow* window, int width, int height)
{
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwFrameBufferResize(QResizeEvent(QSize(width,height),QSize()));
}
static void _glfw_winevent_focus(GLFWwindow* window, int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //We might want to elaborate on the focus reason
    rend->glfwWinFocusChanged((val==1) ? QFocusEvent(QEvent::FocusIn,Qt::MouseFocusReason) : QFocusEvent(QEvent::FocusOut,Qt::MouseFocusReason));
}
static void _glfw_winevent_pos(GLFWwindow* window, int x,int y){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwWinPosChanged(QMoveEvent(QPoint(x,y),QPoint()));
}
static void _glfw_winevent_refresh(GLFWwindow* window){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    rend->glfwWinRefresh();
}
static void _glfw_winevent_close(GLFWwindow* window){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    glfwSetWindowShouldClose(window,0);
    rend->glfwWinClose();
}
static void _glfw_winevent_state(GLFWwindow* window, int val){
    CoffeeRenderer* rend = (CoffeeRenderer*)glfwGetWindowUserPointer(window);
    //Not very accurate
    rend->glfwWinStateChanged((val==1) ? QWindowStateChangeEvent(Qt::WindowMinimized) : QWindowStateChangeEvent(Qt::WindowMaximized));
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_VISIBLE,0);
    glfwWindowHint(GLFW_RESIZABLE,1);

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
    if(window==NULL)
        return 10;

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

    glfwMakeContextCurrent(window);
    //I was told this GLEW is no good, let's swap it.
//    glewExperimental = GL_TRUE;
//    if(glewInit()!=GLEW_OK)
//        return 11;
//    if(!GLEW_VERSION_3_3)
//        return 12;
    //Whew! What little effort!
    glbinding::Binding::initialize(false);
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
    CoffeeCamera camera(this,1.6,0.1,100.0,90.0,glm::vec3(0,0,5),glm::vec3(0,0,0));
    CoffeeWorldOpts world(this);
    world.setCamera(&camera);

    CoffeeJoystick js(this,GLFW_JOYSTICK_1);

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
