#ifndef CFRAMEBUFFER
#define CFRAMEBUFFER

#include "glbinding.h"
#include "coffee.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CFramebuffer
{
    GLuint      handle          = 0;
    CSize       size;

    void create(){
        glGenFramebuffers(1,&handle);
    }
    void free(){
        glDeleteFramebuffers(1,&handle);
    }
};


}
}

#endif // CFRAMEBUFFER

