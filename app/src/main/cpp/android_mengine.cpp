#include "mengine_platform.h"

#include <sys/mman.h> // Used for munmap and mmap

#include "android_mengine.h"

global_variable b32 GlobalRunning;

// NOTE: Taken from google's native activity example
// https://github.com/googlesamples/android-ndk/blob/master/native-activity/app/src/main/cpp/main.cpp
internal int AndroidInitDisplay(android_display_info *DisplayInfo, android_app *AndroidApp)
{
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
	for(; ConfigIndex < NumConfigs; ConfigIndex++)
	{
		EGLConfig &Cfg = SupportedConfigs[ConfigIndex];
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
	if(ConfigIndex == NumConfigs)
	{
		Config = SupportedConfigs[0];
	}

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(Display, Config, EGL_NATIVE_VISUAL_ID, &Format);
	Surface = eglCreateWindowSurface(Display, Config, AndroidApp->window, NULL);
	Context = eglCreateContext(Display, Config, NULL, NULL);

	if(eglMakeCurrent(Display, Surface, Surface, Context) == EGL_FALSE)
	{
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(Display, Surface, EGL_WIDTH, &Width);
	eglQuerySurface(Display, Surface, EGL_HEIGHT, &Height);

	DisplayInfo->Display = Display;
	DisplayInfo->Context = Context;
	DisplayInfo->Surface = Surface;
	DisplayInfo->Width = Width;
	DisplayInfo->Height = Height;

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

internal void AndroidDestroyDisplay(android_display_info *DisplayInfo)
{
	if(DisplayInfo->Display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(DisplayInfo->Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if(DisplayInfo->Context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(DisplayInfo->Display, DisplayInfo->Context);
		}
		if(DisplayInfo->Surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(DisplayInfo->Display, DisplayInfo->Surface);
		}
		eglTerminate(DisplayInfo->Display);
	}

	GlobalRunning = false;
	DisplayInfo->Display = EGL_NO_DISPLAY;
	DisplayInfo->Context = EGL_NO_CONTEXT;
	DisplayInfo->Surface = EGL_NO_SURFACE;
}

internal void AndroidDisplayBufferInWindow(android_display_info *DisplayInfo)
{
	if(DisplayInfo->Display == NULL)
	{
		LOGW("Attempted to render without a display to render to. Skipping render...");
		return;
	}

	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(DisplayInfo->Display, DisplayInfo->Surface);
}

internal void AndroidProcessEvent(android_app *AndroidApp, int32_t Command)
{
	android_state *State = (android_state *)AndroidApp->userData;
	switch(Command)
	{
		case APP_CMD_INIT_WINDOW:
		{
			LOGI("COMMAND: Init Window");
			if(AndroidApp->window != NULL)
			{
				AndroidInitDisplay(&State->DisplayInfo, AndroidApp);
				AndroidDisplayBufferInWindow(&State->DisplayInfo);
				GlobalRunning = true;
			}
		} break;
		case APP_CMD_TERM_WINDOW:
		{
			LOGI("COMMAND: Term Window");
			AndroidDestroyDisplay(&State->DisplayInfo);
		} break;
		case APP_CMD_GAINED_FOCUS:
		{
			// NOTE: Begin doing background work
		} break;
		case APP_CMD_LOST_FOCUS:
		{
			// NOTE: Stop doing background work
			GlobalRunning = false;
		} break;
	}
}

r64 AndroidGetWallClock()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return 1000.0 * tp.tv_sec + (u64)tp.tv_nsec / 1e6;
}

void android_main(struct android_app *AndroidApp)
{
	android_state State = {0};
	AndroidApp->userData = &State;
	AndroidApp->onAppCmd = AndroidProcessEvent;

	while(1)
	{
		// Poll Events
		{
			android_poll_source *Source;
			int ident;
			int events;
			while((ident = ALooper_pollAll(GlobalRunning ? 0 : -1, NULL, &events, (void **)&Source)) >= 0)
			{
				// Process the event
				if(Source != NULL)
				{
					Source->process(AndroidApp, Source);
				}

				// Check if exiting
				if(AndroidApp->destroyRequested != 0)
				{
					AndroidDestroyDisplay(&State.DisplayInfo);
					return;
				}
			}
		}

		if(GlobalRunning)
		{
			r64 frameStart = AndroidGetWallClock();
			AndroidDisplayBufferInWindow(&State.DisplayInfo);
			r64 frameEnd = AndroidGetWallClock();

			r64 frameDuration = frameEnd - frameStart;
			LOGI("Frame Duration: %fms", frameStart, frameEnd, frameDuration);
		}
	}
}
