#include "mengine_platform.h"

#include <android/log.h>
#include <android_native_app_glue.h>

#include "android_mengine.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

// NOTE: Taken from google's native activity example
// https://github.com/googlesamples/android-ndk/blob/master/native-activity/app/src/main/cpp/main.cpp
internal int engine_init_display(android_window_info *WindowInfo, android_app *AndroidApp) {
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint Attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint Width, Height, Format;
	EGLint NumConfigs;
	EGLConfig Config;
	EGLSurface Surface;
	EGLContext Context;

	EGLDisplay Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(Display, 0, 0);

	/* Here, the application chooses the configuration it desires.
	 * find the best match if possible, otherwise use the very first one
	 */
	eglChooseConfig(Display, Attribs, NULL, 0, &NumConfigs);
	EGLConfig SupportedConfigs[NumConfigs];
	eglChooseConfig(Display, Attribs, SupportedConfigs, NumConfigs, &NumConfigs);
	u32 ConfigIndex = 0;
	for (; ConfigIndex < NumConfigs; ConfigIndex++) {
		EGLConfig& Cfg = SupportedConfigs[ConfigIndex];
		EGLint r, g, b, d;
		if (eglGetConfigAttrib(Display, Cfg, EGL_RED_SIZE, &r)   &&
			eglGetConfigAttrib(Display, Cfg, EGL_GREEN_SIZE, &g) &&
			eglGetConfigAttrib(Display, Cfg, EGL_BLUE_SIZE, &b)  &&
			eglGetConfigAttrib(Display, Cfg, EGL_DEPTH_SIZE, &d) &&
			r == 8 && g == 8 && b == 8 && d == 0 ) {

			Config = SupportedConfigs[ConfigIndex];
			break;
		}
	}
	if (ConfigIndex == NumConfigs) {
		Config = SupportedConfigs[0];
	}

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(Display, Config, EGL_NATIVE_VISUAL_ID, &Format);
	Surface = eglCreateWindowSurface(Display, Config, AndroidApp->window, NULL);
	Context = eglCreateContext(Display, Config, NULL, NULL);

	if (eglMakeCurrent(Display, Surface, Surface, Context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(Display, Surface, EGL_WIDTH, &Width);
	eglQuerySurface(Display, Surface, EGL_HEIGHT, &Height);

	WindowInfo->Display = Display;
	WindowInfo->Context = Context;
	WindowInfo->Surface = Surface;
	WindowInfo->Width = Width;
	WindowInfo->Height = Height;

	// Check openGL on the system
	GLenum OpenGLInfo[] = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
	for(s32 InfoIndex = 0; InfoIndex < ArrayCount(OpenGLInfo); ++InfoIndex)
	{
		GLenum Info = OpenGLInfo[InfoIndex];
		const GLubyte *InfoValue = glGetString(Info);
		LOGI("OpenGL Info: %s", InfoValue);
	}
	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

void android_main(struct android_app *state)
{
	for(int x = 0; x < 100; ++x)
	{
		LOGI("Logging %d", x);
	}
	LOGW("Warning! Ending Test");
}
