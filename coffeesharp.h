//These headers are exported to CoffeeSharp
//A lot of C++ code/headers will not work, such as glbinding, glm and *insert any framework*.
//In other words, it is a simple API.

//Core classes that work
#include "coffee/display/cdrendererbase.h"
#include "coffee/cinput.h"
#include "coffee/cdebug.h"

//These don't work or don't export anything useful (Clang errors)
//#include "coffee/cfunctional.h"
//#include "coffee_impl/context/cdrenderer.h"
//#include "coffee_impl/graphics/cgraphicswrappers.h"
