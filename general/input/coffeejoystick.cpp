#include "coffeejoystick.h"

CoffeeJoystick::CoffeeJoystick(QObject *parent, int joystick) : QObject(parent)
{
    this->joystick = joystick;
    j_name = QString(glfwGetJoystickName(joystick));
    int axes;
    int buttons;
    setObjectName("jsdev");
    glfwGetJoystickAxes(joystick,&axes);
    glfwGetJoystickButtons(joystick,&buttons);
    this->axes.resize(axes);
    this->buttons.resize(buttons);
}

CoffeeJoystick::~CoffeeJoystick()
{

}

bool CoffeeJoystick::update()
{
    if(glfwJoystickPresent(joystick)==0||!enabled())
        return false;

    int count;
    const float* c_axes = glfwGetJoystickAxes(joystick,&count);
    this->axes.resize(count); //Sadly, this is necessary.
    for(int i=0;i<count;i++){
        float value = c_axes[i];
        float oldval = axes.at(i);
        if(std::abs(value)<=j_deadzone)
            value = 0;
        axes.replace(i,value);
        axisMoved(i,value,value-oldval);
    }
    const unsigned char* c_btns = glfwGetJoystickButtons(joystick,&count);
    this->buttons.resize(count);
    for(int i=0;i<count;i++)
        if(c_btns[i]!=buttons.at(i)){
            buttons.replace(i,c_btns[i]);
            if(c_btns[i]==GLFW_PRESS)
                buttonPressed(i);
            else
                buttonReleased(i);
        }
    return true;
}
QString CoffeeJoystick::getJoystickName() const
{
    return j_name;
}
float CoffeeJoystick::getSensitivity() const
{
    return j_sensitivity;
}

void CoffeeJoystick::setSensitivity(float value)
{
    j_sensitivity = value;
}
float CoffeeJoystick::getDeadzone() const
{
    return j_deadzone;
}

void CoffeeJoystick::setDeadzone(float value)
{
    j_deadzone = value;
}
float CoffeeJoystick::getAxisfactor() const
{
    return j_axisfactor;
}

void CoffeeJoystick::setAxisfactor(float value)
{
    j_axisfactor = value;
}

int CoffeeJoystick::getAxes() const
{
    return axes.size();
}

int CoffeeJoystick::getButtons() const
{
    return buttons.size();
}

QVariantList CoffeeJoystick::axesValues() const
{
    QVariantList vals;
    for(float v : axes)
        vals << v;
    return vals;
}

QVariantList CoffeeJoystick::btnValues() const
{
    QVariantList vals;
    for(unsigned char v : buttons)
        vals << v;
    return vals;
}

bool CoffeeJoystick::enabled() const
{
    return m_enabled;
}

void CoffeeJoystick::setEnabled(bool enabled)
{
    m_enabled = enabled;
}
