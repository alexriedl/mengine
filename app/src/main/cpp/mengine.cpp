#include "mengine.h"

extern "C" GAME_UPDATE(GameUpdate)
{
}

extern "C" GAME_RENDER(GameRender)
{
	// TODO: Actually initialize render group correctly
	render_group Group = {0};

	// NOTE: These values seem to require knowing the size of the worled...
	PushTexturedRectangle(&Group, 0, 5, 5, 10, 10);
}
