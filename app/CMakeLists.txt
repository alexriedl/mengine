# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-write-strings -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-strict-aliasing -Wno-switch -fno-rtti -fno-exceptions -Wno-sign-compare -std=gnu++11 -Wno-null-dereference -Wno-missing-braces -fno-exceptions -fno-rtti -Wno-switch -Wno-format-security")


# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED src/main/cpp/android_mengine.cpp)
add_library(mengine SHARED src/main/cpp/mengine.cpp)

target_include_directories(native-activity PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
target_link_libraries(
    mengine
    EGL
    GLESv1_CM)

target_link_libraries(
    native-activity
    android
    native_app_glue
    EGL
    GLESv1_CM
    log)
