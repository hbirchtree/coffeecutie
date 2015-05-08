#include "coffeejoystick.h"

CoffeeJoystick::CoffeeJoystick(QObject *parent, int joystick) : QObject(parent)
{
    this->joystick = joystick;
    j_name = QString(glfwGetJoystickName(joystick));
    int axes;
    int buttons;
    glfwGetJoystickAxes(joystick,&axes);
    glfwGetJoystickButtons(joystick,&buttons);
    for(int i=0;i<axes;i++)
        this->axes.append(0);
    for(int i=0;i<buttons;i++)
        this->buttons.append(0);
}

CoffeeJoystick::~CoffeeJoystick()
{

}

bool CoffeeJoystick::update()
{
    if(glfwJoystickPresent(joystick)==0)
        return false;

    int count;
    const float* c_axes = glfwGetJoystickAxes(joystick,&count);
    for(int i=0;i<count;i++){
        float value = c_axes[i];
        float oldval = axes.at(i);
        if(std::abs(value)<=j_deadzone)
            value = 0;
        if(std::abs(value-oldval)<j_sensitivity)
            continue;
        axes.replace(i,value);
        axisMoved(i,value,value-oldval);
    }
    const unsigned char* c_btns = glfwGetJoystickButtons(joystick,&count);
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



