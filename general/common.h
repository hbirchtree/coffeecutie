//Includes
#include <QPointer>
#include <QDebug>
//#include <QMessageLogger>
#include <QThread>
#include <QThreadPool>
#include <QEventLoop>

#include <iostream>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#ifndef QOPENGL_CONTEXT_MANAGER
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#else
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#endif

#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <glbinding/gl/gl.h>

using namespace gl;

//#include "qstringfunctions.h"
#include "engine/data/coffeeresource.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

//#ifndef QUICK_MATH_HELPER
//#define QUICK_MATH_HELPER
//class QuickMath {
//public:
//    static constexpr float Math_Pi = 3.141592654;
//    static constexpr float Math_DegToRadFactor(){
//        return Math_Pi/180;
//    }
//    static constexpr float Math_RadToDegFactor(){
//        return 180/Math_Pi;
//    }
//    static float math_degreesToRads(float deg){
//        return deg*Math_DegToRadFactor();
//    }
//    static float math_radiansToDegs(float rad){
//        return rad*Math_RadToDegFactor();
//    }

//private:
//    QuickMath();
//};
//#endif
