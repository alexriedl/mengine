#include "mengine_platform.h"

#include <sys/mman.h> // Used for munmap and mmap

#include "sdl_mengine.h"

global_variable b32 GlobalRunning;
global_variable b32 GlobalPaused;

global_variable b32 DEBUGGlobalShowCursor;

b32 SDLInitDisplay()
{
	SDL_Init(SDL_INIT_VIDEO);

	// int WindowWidth = 960; int WindowHeight = 540;
	int WindowWidth = 1920; int WindowHeight = 1080;
	SDL_Window *Window = SDL_CreateWindow("mengine",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WindowWidth, WindowHeight,
		SDL_WINDOW_RESIZABLE);
	if(!Window) return false;

	SDL_ShowCursor(DEBUGGlobalShowCursor ? SDL_ENABLE : SDL_DISABLE);

	SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);
	if(!Renderer) return false;

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

internal void SDLProcessPendingEvents()
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
						printf("SDL_WindowEVENT_SIZE_CHANGED (%d, %d)\n", Event.window.data1, Event.window.data2);
					}
					break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					{
						printf("SDL_WindowEVENT_FOCUS_GAINED\n");
					}
					break;
					case SDL_WINDOWEVENT_EXPOSED:
					{
						SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
						SDL_Renderer *Renderer = SDL_GetRenderer(Window);
						// sdl_window_dimension Dimension = SDLGetWindowDimension(Window);
						// SDLDisplayBufferInWindow(&GlobalBackbuffer, Renderer, Dimension.Width, Dimension.Height);
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

int main()
{
#if MENGINE_INTERNAL
	DEBUGGlobalShowCursor = true;
#endif

	if(SDLInitDisplay())
	{
		GlobalRunning = true;
		while(GlobalRunning)
		{
			SDLProcessPendingEvents();

			if(!GlobalPaused)
			{
				printf("LOOOOOP\n");
			}
		}
	}
}
