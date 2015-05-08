//Includes
#include <QPointer>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QMessageLogger>
#include <QImage>
#include <QThread>
#include <QHash>
#include <QListIterator>
#include <QDateTime>

#include <iostream>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>

#include <glbinding/gl/gl.h>

using namespace gl;

#ifndef QUICK_MATH_HELPER
#define QUICK_MATH_HELPER
class QuickMath {
public:
    static constexpr float Math_Pi = 3.141592654;
    static constexpr float Math_DegToRadFactor(){
        return Math_Pi/180;
    }
    static constexpr float Math_RadToDegFactor(){
        return 180/Math_Pi;
    }
    static float math_degreesToRads(float deg){
        return deg*Math_DegToRadFactor();
    }
    static float math_radiansToDegs(float rad){
        return rad*Math_RadToDegFactor();
    }

private:
    QuickMath();
};
#endif
