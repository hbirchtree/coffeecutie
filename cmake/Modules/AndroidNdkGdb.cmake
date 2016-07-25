# Copyright (c) 2014, Pavel Rojtberg
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# ------------------------------------------------------------------------------
# Usage:
# 1. place AndroidNdkGdb.cmake somewhere inside ${CMAKE_MODULE_PATH}
# 2. inside your project add
#
#    include(AndroidNdkGdb)
#    android_ndk_gdb_enable()
#    # for each target
#    add_library(MyLibrary ...)
#    android_ndk_gdb_debuggable(MyLibrary)


# add gdbserver and general gdb configuration to project
# also create a mininal NDK skeleton so ndk-gdb finds the paths
#
# the optional parameter defines the path to the android project.
# uses PROJECT_SOURCE_DIR by default.

set ( ANDROID_GDBSERVER_BINARY CACHE FILEPATH "" )

macro(android_ndk_gdb_enable TARGET_NAME PROJECT_DIR DEPLOY_DIR SOURCE_FILES LIBRARIES)
    if(ANDROID)
        # create custom target that depends on the real target so it gets executed afterwards
        add_custom_target("${TARGET_NAME}_NDK_GDB" ALL)

        set(ANDROID_PROJECT_DIR ${PROJECT_DIR})

        set(NDK_GDB_SOLIB_PATH ${ANDROID_PROJECT_DIR}/obj/local/${ANDROID_NDK_ABI_NAME}/)
        file(MAKE_DIRECTORY ${NDK_GDB_SOLIB_PATH})

        # 1. generate essential Android Makefiles
        file(MAKE_DIRECTORY ${ANDROID_PROJECT_DIR}/jni)
        if(NOT EXISTS ${ANDROID_PROJECT_DIR}/jni/Android.mk)
            file(WRITE ${ANDROID_PROJECT_DIR}/jni/Android.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()
        if(NOT EXISTS ${ANDROID_PROJECT_DIR}/jni/Application.mk)
            file(WRITE ${ANDROID_PROJECT_DIR}/jni/Application.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()

        # 2. generate gdb.setup
        get_directory_property(PROJECT_INCLUDES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} INCLUDE_DIRECTORIES)
        string(REGEX REPLACE ";" " " PROJECT_INCLUDES "${PROJECT_INCLUDES}")
        file(WRITE ${DEPLOY_DIR}/gdb.setup "set solib-search-path ${NDK_GDB_SOLIB_PATH}\n")
        file(APPEND ${DEPLOY_DIR}/gdb.setup "directory ${PROJECT_INCLUDES}\n")

        # 2.1 generate gdb.host.setup, to run on the host computer
        file(WRITE ${PROJECT_DIR}/gdb.host.setup "set solib-search-path ${NDK_GDB_SOLIB_PATH}\n")
        file(APPEND ${PROJECT_DIR}/gdb.host.setup "directory ${PROJECT_INCLUDES}\n")
        file(APPEND ${PROJECT_DIR}/gdb.host.setup "file obj/local/${ANDROID_NDK_ABI_NAME}/lib${TARGET_NAME}.so\n" )
        file(APPEND ${PROJECT_DIR}/gdb.host.setup "target extended-remote :5052\n")
        file(APPEND ${PROJECT_DIR}/gdb.host.setup "set remote exec-file /system/bin/am\n" )
        file(APPEND ${PROJECT_DIR}/gdb.host.setup
            "run am -a ${ANDROID_AM_START_INTENT} -n ${ANDROID_AM_START_ACTIVITY}\n" )

        # 3. copy gdbserver executable
        configure_file (
            "${ANDROID_NDK}/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver"
            "${DEPLOY_DIR}/gdbserver"
            COPYONLY
            )
        configure_file (
            "${DEPLOY_DIR}/gdbserver"
            "${DEPLOY_DIR}/libgdbserver.so"
            COPYONLY
            )

    endif()
endmacro()

# register a target for remote debugging
# copies the debug version to NDK_GDB_SOLIB_PATH then strips symbols of original
macro(android_ndk_gdb_debuggable TARGET_NAME)
    if(ANDROID)
        # create custom target that depends on the real target so it gets executed afterwards
        add_dependencies("${TARGET_NAME}_NDK_GDB" ${TARGET_NAME})
    endif()
endmacro()

macro(android_ndk_gdb_debug TARGET_NAME PROJECT_DIR DEVICE_TARGET )
    message ( "-- Creating GDB target for ${TARGET_NAME}" )
    set ( INT_TARGET "${TARGET_NAME}_RD" )
    add_custom_target ( "${INT_TARGET}" )
    set_target_properties("${INT_TARGET}" PROPERTIES EXCLUDE_FROM_ALL TRUE)

    execute_process (
        COMMAND adb -s "${DEVICE_TARGET}" push "${ANDROID_GDBSERVER_BINARY}" "/data/local/gdbserver"
        )

    add_custom_command (
        TARGET "${INT_TARGET}"
        COMMAND adb -s "${DEVICE_TARGET}" forward tcp:5052 tcp:5052
        )
    add_custom_command (
        TARGET "${INT_TARGET}"
        COMMAND adb -s "${DEVICE_TARGET}" shell /data/local/gdbserver --multi :5052
        )
endmacro()
