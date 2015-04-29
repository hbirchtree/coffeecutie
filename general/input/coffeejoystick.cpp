#include "coffeejoystick.h"

CoffeeJoystick::CoffeeJoystick(QObject *parent, int joystick) : QObject(parent)
{
    this->joystick = joystick;
    joystickName = QString(glfwGetJoystickName(joystick));
}

CoffeeJoystick::~CoffeeJoystick()
{

}

bool CoffeeJoystick::update()
{
    if(glfwJoystickPresent(joystick)==0)
        return false;

    axes.clear();
    buttons.clear();

    int count;
    const float* c_axes = glfwGetJoystickAxes(joystick,&count);
    for(int i=0;i<count;i++)
        axes.append(c_axes[i]);
    const unsigned char* c_btns = glfwGetJoystickButtons(joystick,&count);
    for(int i=0;i<count;i++){
        if(c_btns[i]==GLFW_PRESS)
            buttons.append(true);
        else
            buttons.append(false);
    }
    return true;
}
QString CoffeeJoystick::getJoystickName() const
{
    return joystickName;
}
QList<float> CoffeeJoystick::getAxes() const
{
    return axes;
}
QList<bool> CoffeeJoystick::getButtons() const
{
    return buttons;
}
