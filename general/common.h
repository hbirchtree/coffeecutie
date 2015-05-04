//Includes
#include <QPointer>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QThread>
#include <QHash>
#include <QListIterator>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    static float math_degreesToRads(float deg){
        return deg*Math_Pi/180;
    }
    static float math_radiansToDegs(float rad){
        return rad/Math_Pi*180;
    }
    static glm::vec3 convert_quat_euler(glm::quat q){
        return glm::vec3(0,0,0);
    }
    static glm::quat convert_euler_quat(glm::vec3 e){
        return glm::quat(0,0,0,0);
    }

private:
    QuickMath();
};
#endif
