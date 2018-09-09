/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>

#include "JNIHelper.h"

namespace ndk_helper
{

#define CLASS_NAME "android/app/NativeActivity"

//---------------------------------------------------------------------------
//JNI Helper functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Singleton
//---------------------------------------------------------------------------
JNIHelper* JNIHelper::GetInstance()
{
    static JNIHelper helper;
    return &helper;
}

//---------------------------------------------------------------------------
//Ctor
//---------------------------------------------------------------------------
JNIHelper::JNIHelper()
{
    pthread_mutex_init( &mutex_, NULL );
}

//---------------------------------------------------------------------------
//Dtor
//---------------------------------------------------------------------------
JNIHelper::~JNIHelper()
{
    pthread_mutex_lock( &mutex_ );

    JNIEnv *env;
    activity_->vm->AttachCurrentThread( &env, NULL );

    env->DeleteGlobalRef( jni_helper_java_ref_ );
    env->DeleteGlobalRef( jni_helper_java_class_ );

    activity_->vm->DetachCurrentThread();

    pthread_mutex_destroy( &mutex_ );
}

//---------------------------------------------------------------------------
//Init
//---------------------------------------------------------------------------
void JNIHelper::Init( ANativeActivity* activity,
        const char* helper_class_name )
{
    JNIHelper& helper = *GetInstance();
    pthread_mutex_lock( &helper.mutex_ );

    helper.activity_ = activity;

    JNIEnv *env;
    helper.activity_->vm->AttachCurrentThread( &env, NULL );

    //Retrieve app name
    jclass android_content_Context = env->GetObjectClass( helper.activity_->clazz );
    jmethodID midGetPackageName = env->GetMethodID( android_content_Context, "getPackageName",
            "()Ljava/lang/String;" );

    jstring packageName = (jstring) env->CallObjectMethod( helper.activity_->clazz,
            midGetPackageName );
    const char* appname = env->GetStringUTFChars( packageName, NULL );
    helper.app_name_ = std::string( appname );

    jclass cls = helper.RetrieveClass( env, helper_class_name );
    helper.jni_helper_java_class_ = (jclass) env->NewGlobalRef( cls );

    jmethodID constructor = env->GetMethodID( helper.jni_helper_java_class_, "<init>", "()V" );
    helper.jni_helper_java_ref_ = env->NewObject( helper.jni_helper_java_class_, constructor );
    helper.jni_helper_java_ref_ = env->NewGlobalRef( helper.jni_helper_java_ref_ );

    env->ReleaseStringUTFChars( packageName, appname );
    helper.activity_->vm->DetachCurrentThread();

    pthread_mutex_unlock( &helper.mutex_ );
}

//---------------------------------------------------------------------------
//Audio helpers
//---------------------------------------------------------------------------
int32_t JNIHelper::GetNativeAudioBufferSize()
{
    if( activity_ == NULL )
    {
        LOGI( "JNIHelper has not been initialized. Call init() to initialize the helper" );
        return 0;
    }

    JNIEnv *env;
    jmethodID mid;

    pthread_mutex_lock( &mutex_ );
    activity_->vm->AttachCurrentThread( &env, NULL );

    mid = env->GetMethodID( jni_helper_java_class_, "getNativeAudioBufferSize", "()I" );
    int32_t i = env->CallIntMethod( jni_helper_java_ref_, mid );
    activity_->vm->DetachCurrentThread();
    pthread_mutex_unlock( &mutex_ );

    return i;
}

int32_t JNIHelper::GetNativeAudioSampleRate()
{
    if( activity_ == NULL )
    {
        LOGI( "JNIHelper has not been initialized. Call init() to initialize the helper" );
        return 0;
    }

    JNIEnv *env;
    jmethodID mid;

    pthread_mutex_lock( &mutex_ );
    activity_->vm->AttachCurrentThread( &env, NULL );

    mid = env->GetMethodID( jni_helper_java_class_, "getNativeAudioSampleRate", "()I" );
    int32_t i = env->CallIntMethod( jni_helper_java_ref_, mid );
    activity_->vm->DetachCurrentThread();
    pthread_mutex_unlock( &mutex_ );

    return i;
}

//---------------------------------------------------------------------------
//Misc implementations
//---------------------------------------------------------------------------
jclass JNIHelper::RetrieveClass( JNIEnv *jni,
        const char* class_name )
{
    jclass activity_class = jni->FindClass( CLASS_NAME );
    jmethodID get_class_loader = jni->GetMethodID( activity_class, "getClassLoader",
            "()Ljava/lang/ClassLoader;" );
    jobject cls = jni->CallObjectMethod( activity_->clazz, get_class_loader );
    jclass class_loader = jni->FindClass( "java/lang/ClassLoader" );
    jmethodID find_class = jni->GetMethodID( class_loader, "loadClass",
            "(Ljava/lang/String;)Ljava/lang/Class;" );

    jstring str_class_name = jni->NewStringUTF( class_name );
    jclass class_retrieved = (jclass) jni->CallObjectMethod( cls, find_class, str_class_name );
    jni->DeleteLocalRef( str_class_name );
    return class_retrieved;
}

jstring JNIHelper::GetExternalFilesDirJString( JNIEnv *env )
{
    if( activity_ == NULL )
    {
        LOGI( "JNIHelper has not been initialized. Call init() to initialize the helper" );
        return NULL;
    }

    // Invoking getExternalFilesDir() java API
    jclass cls_Env = env->FindClass( CLASS_NAME );
    jmethodID mid = env->GetMethodID( cls_Env, "getExternalFilesDir",
            "(Ljava/lang/String;)Ljava/io/File;" );
    jobject obj_File = env->CallObjectMethod( activity_->clazz, mid, NULL );
    jclass cls_File = env->FindClass( "java/io/File" );
    jmethodID mid_getPath = env->GetMethodID( cls_File, "getPath", "()Ljava/lang/String;" );
    jstring obj_Path = (jstring) env->CallObjectMethod( obj_File, mid_getPath );

    return obj_Path;
}

} //namespace ndkHelper
