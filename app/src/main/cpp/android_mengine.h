#if !defined(ANDROID_MENGINE_H)

#include <android/log.h>

#if MENGINE_SLOW
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))
#else
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

struct android_game_code
{
	game_update_and_render *UpdateAndRender;

	void *GameCodeDll;
	time_t DllLastWriteTime;
	b32 IsValid;
};

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
