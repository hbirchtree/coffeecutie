#include <coffee/android/android_main.h>
#include <coffee/jni/jnipp.h>

#include <android_native_app_glue.h>

namespace jnipp {

thread_local android::ScopedJNI* jniScope = nullptr;

android::ScopedJNI* SwapJNI(android::ScopedJNI* jniScope)
{
    auto prevScope  = jniScope;
    jnipp::jniScope = jniScope;

    return prevScope;
}

JNIEnv* GetJNI()
{
    if(!jniScope)
        throw undefined_behavior("no JNI environment");

    return jniScope->env();
}

JavaVM* GetVM()
{
    return Coffee::coffee_app->activity->vm;
}

} // namespace jnipp
