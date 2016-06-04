#include <coffee/core/coffee.h>
#include <coffee/core/coffee_version.h>

#include <coffee/core/CDebug>

#include <android/sensor.h>

#include <android_native_app_glue.h>

using namespace Coffee;

extern "C" __attribute__((visibility("default"))) void CoffeeActivity_onCreate(
        ANativeActivity* activity,
        void* savedState,
        size_t savedStateSize
)
{
    /*
     * We do this because the visibility of the proper function
     *  declaration is not visible with -fvisibility=hidden, this
     *  allows us to keep the NDK as it is while also having a
     *  nice entry point for other code.
     * */
    ANativeActivity_onCreate(activity,savedState,savedStateSize);
}

extern CoffeeMainWithArgs android_entry_point;

void android_main(struct android_app* state)
{
    /* According to docs, something something glue check */
    app_dummy();

    cDebug("User data pointer: {0}",(const void* const&)state->userData);

    state->userData = state;

    cDebug("Asset manager: {0}",(const void* const&)state->activity->assetManager);
    cDebug("Android API version: {0}",state->activity->sdkVersion);

    cDebug("Android data directory: {0}",state->activity->internalDataPath);
    cDebug("Android external data directory: {0}",state->activity->externalDataPath);

    AAsset* test_ass = AAssetManager_open(state->activity->assetManager,"dir/file.test",AASSET_MODE_BUFFER);

    if(test_ass)
    {
        cDebug("Asset size: {0}",AAsset_getLength(test_ass));

        CString data;
        data.insert(0,(cstring)AAsset_getBuffer(test_ass),AAsset_getLength(test_ass));

        cDebug("Asset text: {0}",data);

        AAsset_close(test_ass);
    }else{
        cDebug("Failed to open test_ass :(");
    }

    {
        /* Set version information */
        CoffeeApplicationData.application_name = COFFEE_APPLICATION_NAME;
        CoffeeApplicationData.organization_name = COFFEE_ORGANIZATION_NAME;
        CoffeeApplicationData.version_code = COFFEE_VERSION_CODE;

        /* Get application name, just stock */
        cstring_w appname = &(CoffeeApplicationData.application_name[0]);

        /* And then load the usual main() entry point */
        if(android_entry_point)
        {
            cDebug("Entry point located, entering");
            int32 status = CoffeeMain(android_entry_point,1,&appname);
            cDebug("Android exit: {0}",status);
        }else{
            cWarning("Failed to load application entry point!");
        }
    }
}
