#include "mengine_platform.h"

#include <sys/stat.h> // Used to get file info
#include <unistd.h>   // readlink
#include <dlfcn.h>    // Used to load dynamic library
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "sdl_mengine.h"

global_variable b32 GlobalRunning;
global_variable b32 GlobalPaused;

global_variable b32 DEBUGGlobalShowCursor;

internal void CatStrings(size_t SourceACount, char *SourceA, size_t SourceBCount, char *SourceB, size_t DestCount,
                         char *Dest)
{
	for(int Index = 0; Index < SourceACount; Index++)
	{
		*Dest++ = *SourceA++;
	}

	for(int Index = 0; Index < SourceBCount; Index++)
	{
		*Dest++ = *SourceB++;
	}

	*Dest++ = 0;
}

internal int StringLength(char *Str)
{
	int Count = 0;
	while(*Str++)
	{
		++Count;
	}

	return Count;
}

b32 SDLInitDisplay(sdl_display_info *DisplayInfo)
{
	SDL_Init(SDL_INIT_VIDEO);

	// int WindowWidth = 960; int WindowHeight = 540;
	int WindowWidth = 1920; int WindowHeight = 1080;
	SDL_Window *Window = SDL_CreateWindow("mengine",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WindowWidth, WindowHeight,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if(!Window) return false;

	SDL_ShowCursor(DEBUGGlobalShowCursor ? SDL_ENABLE : SDL_DISABLE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// VSync
	SDL_GL_SetSwapInterval(1);

	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	if(!Context) return false;

	DisplayInfo->Window = Window;
	DisplayInfo->Context = Context;

	return true;
}

internal void SDLToggleFullscreen(SDL_Window *Window)
{
	uint32 Flags = SDL_GetWindowFlags(Window);
	if(Flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		SDL_SetWindowFullscreen(Window, 0);
	}
	else
	{
		SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
}

internal void SDLSwapBuffers(sdl_display_info *DisplayInfo)
{
	if(DisplayInfo->Window == NULL)
	{
		LOGW("Attempted to render without a display to render to. Skipping render...");
		return;
	}
	SDL_GL_SwapWindow(DisplayInfo->Window);
}

internal sdl_window_dimension SDLGetWindowDimension(sdl_display_info *DisplayInfo)
{
	sdl_window_dimension Result;
	SDL_GetWindowSize(DisplayInfo->Window, &Result.Width, &Result.Height);
	return Result;
}

internal void SDLProcessPendingEvents(sdl_state *State)
{
	SDL_Event Event;
	while(SDL_PollEvent(&Event))
	{
		switch(Event.type)
		{
			case SDL_QUIT:
			{
				GlobalRunning = false;
			}
			break;
			case SDL_WINDOWEVENT:
			{
				switch(Event.window.event)
				{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
					{
						LOGI("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)", Event.window.data1, Event.window.data2);
					}
					break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					{
						LOGI("SDL_WINDOWEVENT_FOCUS_GAINED");
					}
					break;
					case SDL_WINDOWEVENT_EXPOSED:
					{
						SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
						SDL_Renderer *Renderer = SDL_GetRenderer(Window);
						SDLSwapBuffers(&State->DisplayInfo);
					}
					break;
				}
			}
			break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				SDL_Keycode KeyCode = Event.key.keysym.sym;
				bool IsDown = (Event.key.state == SDL_PRESSED);
				if(Event.key.repeat == 0)
				{
#if HANDMADE_INTERNAL
					else if(KeyCode == SDLK_p)
					{
						if(IsDown)
						{
							GlobalPause = !GlobalPause;
						}
					}
#endif

					if(IsDown)
					{
						bool AltKeyWasDown = (Event.key.keysym.mod & KMOD_ALT);
						if(KeyCode == SDLK_F4 && AltKeyWasDown)
						{
							GlobalRunning = false;
						}

						if((KeyCode == SDLK_RETURN) && AltKeyWasDown)
						{
							SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
							if(Window)
							{
								SDLToggleFullscreen(Window);
							}
						}
					}
				}
			}
			break;
		}
	}
}

internal void SDLGetEXEFileName(sdl_state *State)
{
	memset(State->ExeFileName, 0, sizeof(State->ExeFileName));

	ssize_t SizeOfFilename = readlink("/proc/self/exe", State->ExeFileName, sizeof(State->ExeFileName) - 1);
	State->OnePastLastExeFileNameSlash = State->ExeFileName;
	for(char *Scan = State->ExeFileName; *Scan; ++Scan)
	{
		if(*Scan == '/')
		{
			State->OnePastLastExeFileNameSlash = Scan + 1;
		}
	}
}

internal void SDLBuildExePathFileName(sdl_state *State, char *FileName, int DestCount, char *Dest)
{
	CatStrings(State->OnePastLastExeFileNameSlash - State->ExeFileName, State->ExeFileName, StringLength(FileName),
	           FileName, DestCount, Dest);
}

inline time_t SDLGetLastWriteTime(char *FileName)
{
	time_t LastWriteTime = 0;

	struct stat FileStatus;
	if(stat(FileName, &FileStatus) == 0)
	{
		LastWriteTime = FileStatus.st_mtime;
	}

	return LastWriteTime;
}

internal sdl_game_code SDLLoadGameCode(char *SourceDllName)
{
	sdl_game_code Result = {};

	Result.DllLastWriteTime = SDLGetLastWriteTime(SourceDllName);

	if(Result.DllLastWriteTime)
	{
		Result.GameCodeDll = dlopen(SourceDllName, RTLD_LAZY);
		if(Result.GameCodeDll)
		{
			Result.Update = (game_update *)dlsym(Result.GameCodeDll, "GameUpdate");
			Result.Render = (game_render *)dlsym(Result.GameCodeDll, "GameRender");
			Result.IsValid = Result.Update && Result.Render;
		}
		else
		{
			LOGE("%s", dlerror());
		}
	}

	if(!Result.IsValid)
	{
		Result.Update = 0;
		Result.Render = 0;
	}

	return Result;
}

#include "mengine_opengl.h"
#include "mengine_opengl.cpp"

int main()
{
#if MENGINE_INTERNAL
	DEBUGGlobalShowCursor = true;
#endif

	sdl_state State = {0};

	char SourceGameCodeDllFullPath[LINUX_STATE_FILE_NAME_COUNT];
	SDLGetEXEFileName(&State);
	SDLBuildExePathFileName(&State, "mengine.so", sizeof(SourceGameCodeDllFullPath), SourceGameCodeDllFullPath);

	if(SDLInitDisplay(&State.DisplayInfo))
	{
		sdl_game_code Game = SDLLoadGameCode(SourceGameCodeDllFullPath);

		sdl_window_dimension Dimensions = SDLGetWindowDimension(&State.DisplayInfo);
		game_render_commands Commands = DefaultRenderCommands(Dimensions.Width / 2, Dimensions.Height / 2);

		GlobalRunning = true;
		while(GlobalRunning)
		{
			SDLProcessPendingEvents(&State);
			Dimensions = SDLGetWindowDimension(&State.DisplayInfo);

			if(!GlobalPaused)
			{
				if(Game.Update) Game.Update();
				if(Game.Render) Game.Render();
				OpenGLRenderCommands(&Commands, Dimensions.Width, Dimensions.Height);

				SDLSwapBuffers(&State.DisplayInfo);
			}
		}
	}
}
