#ifndef CAPPLICATION_START
#define CAPPLICATION_START
#include "coffee/core/coffee.h"
#include "coffee/core/coffee_types.h"

#define COFFEE_APPLICATION_MAIN(mainfun) \
    int main(int argv, char** argc)try \
    { \
        Coffee::CoffeeInit(); \
        return mainfun(argv,argc); \
    }catch(std::exception e){ \
        return 1; \
    } \

namespace Coffee{
/*!
 * \brief A prototype for Coffee-managed main functions. Benefits to this is that Coffee will abstract away any platform-specific quirks to the main function, such as difference in arguments. (Eg. some platforms do not include the main executable while others do)
 */
typedef int32(*CoffeeMainWithArgs)(int32,byte**);
}

#endif
