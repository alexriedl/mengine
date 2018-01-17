#if !defined(SDL_MENGINE_H)

#if MENGINE_SLOW
#define LOGI(...) printf("(I): " __VA_ARGS__); printf("\n");
#define LOGW(...) printf("(W): " __VA_ARGS__); printf("\n");
#define LOGE(...) printf("(E): " __VA_ARGS__); printf("\n");
#else
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

#define LINUX_STATE_FILE_NAME_COUNT 4096

struct sdl_game_code
{
	game_update *Update;
	game_render *Render;

	void *GameCodeDll;
	time_t DllLastWriteTime;
	b32 IsValid;
};

struct sdl_window_dimension
{
	int Width;
	int Height;
};

struct sdl_display_info
{
	SDL_Window *Window;
	SDL_GLContext Context;
};

struct sdl_state
{
	sdl_display_info DisplayInfo;

	char ExeFileName[LINUX_STATE_FILE_NAME_COUNT];
	char *OnePastLastExeFileNameSlash;
};

#define SDL_MENGINE_H
#endif
