#include <coffee/core/coffee.h>
#include <coffee/core/coffee_version.h>

#include <coffee/core/CDebug>

#include <android/sensor.h>
#include <android/log.h>

#include <android_native_app_glue.h>

using namespace Coffee;

void android_main(struct android_app* state)
{
    {
        cDebug("Hello!");

        Coffee::CoffeeApplicationData.application_name = COFFEE_APPLICATION_NAME;
        Coffee::CoffeeApplicationData.organization_name = COFFEE_ORGANIZATION_NAME;
        Coffee::CoffeeApplicationData.version_code = COFFEE_VERSION_CODE;
    }

    return;
}
