#include <QPointer>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QThread>
#include <QHash>
#include <QListIterator>

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
//#include <GL/glew.h>
//#include "glext.h"
//#include "glcorearb.h"
