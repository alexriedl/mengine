#if !defined(ANDROID_MENGINE_H)

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

struct android_display_info
{
	EGLDisplay Display;
    EGLSurface Surface;
    EGLContext Context;
    s32 Width;
    s32 Height;
};

struct android_state
{
	android_display_info DisplayInfo;
};


#define ANDROID_MENGINE_H
#endif
