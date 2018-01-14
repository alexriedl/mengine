#include "mengine.h"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
