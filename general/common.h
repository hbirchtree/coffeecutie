//Qt-centric
#include <QPointer>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QEventLoop>

//Std-lib-ish
#include <iostream>

//GLM
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

//Context manager
#ifndef QOPENGL_CONTEXT_MANAGER
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#else
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#endif

//GLBinding
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding/callbacks.h>
#include <glbinding/Binding.h>
#include <glbinding/Meta.h>

#include <glbinding/gl/gl.h>

using namespace gl;

//ASSIMP
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

//Coffee
#include "engine/data/coffeeresource.h"
