//#include <unit_tests/data_types.h>

//#include <coffee.h>
//#include <base/cdebug.h>
//#include <base/cmath.h>

//#include <glbinding/gl/gl.h>

//namespace CoffeeTests{

//using namespace Coffee;

//void int_tests()
//{
//    //We want this for safe buffer operations
//    CASSERT((sizeof(gl::GLint)==sizeof(int32)));
//    CASSERT((sizeof(gl::GLint64)==sizeof(int64)));
//    CASSERT((sizeof(gl::GLuint)==sizeof(uint32)));
//    CASSERT((sizeof(gl::GLuint64)==sizeof(uint64)));

//    CASSERT((sizeof(byte_t)==sizeof(gl::GLbyte)));
//    CASSERT((sizeof(byte_t)==sizeof(gl::GLchar)));

//    CASSERT((sizeof(uint8) ==sizeof(char)));
//    CASSERT((sizeof(uint16)==sizeof(short)));
//    CASSERT((sizeof(uint32)==sizeof(int)));
//    CASSERT((sizeof(uint64)==sizeof(long long)));

//    //Check for unwanted integer overflow, ensure consistency
//    uint8 n8 = std::pow(2,8)-1;
//    CASSERT((n8==std::pow(2,8)-1));

//    uint16 n16 = std::pow(2,16)-1;
//    CASSERT((n16==std::pow(2,16)-1));

//    uint32 n32 = std::pow(2,32)-1;
//    CASSERT((n32==std::pow(2,32)-1));
//}

//void floating_tests()
//{
//    CASSERT((sizeof(Coffee::scalar)==sizeof(float)));
//    CASSERT((sizeof(Coffee::bigscalar)==sizeof(double)));

//    CASSERT((sizeof(float)==4));
//    CASSERT((sizeof(double)==8));

//    CASSERT((sizeof(float)==sizeof(gl::GLfloat)));
//    CASSERT((sizeof(double)==sizeof(gl::GLdouble)));
//}

//void run_type_tests(bool silent)
//{
//    if(!silent)
//    {
//        cMsg("Coffee Unit Tests","Matrix tests passed");
//        cMsg("Coffee Unit Tests","Integer tests starting");
//    }
//    int_tests();
//    if(!silent){
//        cMsg("Coffee Unit Tests","Integer tests passed");
//        cMsg("Coffee Unit Tests","Floating-point tests starting");
//    }
//    floating_tests();
//    if(!silent)
//        cMsg("Coffee Unit Tests","Floating-point tests passed");
//}

//}