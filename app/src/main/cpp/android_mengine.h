#if !defined(ANDROID_MENGINE_H)

#include <EGL/egl.h>
#include <GLES/gl.h>

struct android_window_info
{
	EGLDisplay Display;
    EGLSurface Surface;
    EGLContext Context;
    s32 Width;
    s32 Height;
};


#define ANDROID_MENGINE_H
#endif
